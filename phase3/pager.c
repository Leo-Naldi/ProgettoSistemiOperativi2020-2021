#include "pager.h"

#define SWAPSIZE 16

#ifdef __PANDOS_DEBUGGER_ACTIVE__

#include "debugger.h"

#endif

static unsigned int swap_pool_h;                   /* puntatore all'inizio della swap pool */    
static swap_t swap_pool_table[SWAPSIZE];   /* swap table */
static volatile int swap_pool_sem;         /* semaforo per la mutua esclusione sulla swap pool */

#define __GET_FRAME(INDEX) (swap_pool_h + ((INDEX) * PAGESIZE))

static int get_next_swap_index();    /* ritorna l'indice del prossimo frame da usare */

static void get_swap_pool_mutex();      /* wrapper per p(swap_pool_sem) */

static void release_swap_pool_mutex();  /* wrapper per v(swap_pool_sem) */

static int flash_io(unsigned int flash_no, int frame_no, unsigned int block_no, unsigned int command);

static void pagefault_handler(support_t* sup);  /* Pager vero e proprio */

void init_pager()
{
	swap_pool_h = SWAP_POOL_START;   /* Vedi proj_lib/pandos_const.h */
	swap_pool_sem = 1;
	
	int i;
	for (i = 0; i < SWAPSIZE; i++)
	{
		swap_pool_table[i].sw_asid = NOPROC;
		swap_pool_table[i].sw_pageNo = -1;
		swap_pool_table[i].sw_pte = NULL;
	}
}

void pager()
{
	support_t* cur_proc_sup_ptr;    /* puntatore alla struttura di supporto del current_process */
	unsigned int exception_code;    /* codice dell'eccezione */             

	cur_proc_sup_ptr = (support_t*) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
	exception_code = CAUSE_GET_EXCCODE((cur_proc_sup_ptr->sup_exceptState)[PGFAULTEXCEPT].cause);

	switch (exception_code)
	{
		case EXC_MOD:
			
#ifdef __PANDOS_DEBUGGER_ACTIVE__
			
			debug_panic_loc = DEBUG_PAGER_ID + 1;

#endif
			PANIC();   /* per debuggare, non dovrebbe succedere in questa fase */
			break;
		
		case EXC_TLBL:
		case EXC_TLBS:
			
			pagefault_handler(cur_proc_sup_ptr);

			break;
		
		default:
#ifdef __PANDOS_DEBUGGER_ACTIVE__
			
			debug_panic_loc = DEBUG_PAGER_ID + 2;

#endif
			PANIC();   /* chiamato l'handler sbagliato */
			break;
	}
	
#ifdef __PANDOS_DEBUGGER_ACTIVE__
			
	debug_panic_loc = DEBUG_PAGER_ID + 3;

#endif
	PANIC();  /* Should Not arrive here */
}


static void get_swap_pool_mutex()
{
	SYSCALL(PASSEREN, (int) &swap_pool_sem, 0, 0);
}

static void release_swap_pool_mutex()
{
	SYSCALL(VERHOGEN, (int) &swap_pool_sem, 0, 0);
}

static int get_next_swap_index()
{
	static int swap_index = 0;
	int res;

	res = (swap_index + 1) % SWAPSIZE;
	swap_index = res;

	if (swap_pool_table[res].sw_asid != NOPROC)
	{
		int i;

		for (i = res + 1; i < SWAPSIZE; i++)
		{
			if (swap_pool_table[i].sw_asid == NOPROC)
			{
				return i;
			}
		}
		
		for (i = 0; i < res; i++)
		{
			if (swap_pool_table[i].sw_asid == NOPROC)
			{
				return i;
			}
		}
	}

	return res;
}

static int flash_io(unsigned int flash_no, int frame_no, unsigned int block_no, unsigned int command)
{
#ifdef __PANDOS_DEBUGGER_ACTIVE__
			
	if ((command != FLASHREAD) && (command != FLASHWRITE))
	{
		debug_panic_loc = DEBUG_PAGER_ID + 6;
		PANIC();
	}
	
	if (block_no > 31)
	{
		debug_panic_loc = DEBUG_PAGER_ID + 7;
		PANIC();
	}
#endif

	unsigned int frame_ptr,
				 old_status,
				 res;
	dtpreg_t* flas_reg_ptr;

	frame_ptr =  __GET_FRAME(frame_no);
	flas_reg_ptr = (dtpreg_t*) GET_DEVREG_ADDR(4, flash_no);

	old_status = getSTATUS();
	setSTATUS(old_status & (~IECON) & (~TEBITON));
	
	flas_reg_ptr->data0 = frame_ptr;
	flas_reg_ptr->command = (block_no << 8) | command;
	res = SYSCALL(IOWAIT, 4, flash_no, 0);

	setSTATUS(old_status);

	return res;
}

static void invalid_tlb_entry(unsigned int entry_hi, unsigned int new_entry_hi, unsigned int new_entry_lo)
{
	unsigned int old_entry_hi;
	unsigned int old_entry_lo;

	old_entry_hi = getENTRYHI();
	old_entry_lo = getENTRYLO();

	setENTRYHI(entry_hi);
	TLBP();

	if ((getINDEX() & 0x80000000) == 0)
	{
		setENTRYHI(new_entry_hi);
		setENTRYLO(new_entry_lo);

		TLBWI();
	}

	setENTRYHI(old_entry_hi);
	setENTRYLO(old_entry_lo);
}

static void pagefault_handler(support_t* sup)
{
#ifdef __PANDOS_DEBUGGER_ACTIVE__

	debug_pager_count++;

#endif

	unsigned int page_no,    /* Logical page number */
		page_index,          /* Indice della pagina */
		entry_hi,            /* EntryHi reg di caller */
		next_frame_index,    /* Indice del prossimo ram frame */ 
		io_val;            /* 1 se un'operazione di scrittura/lettura del flash e' fallita */
	
	state_t* caller;             /* Exception state */
	unsigned int old_status;     /* Usata per spegnere gli interrupt */
	pteEntry_t* page_pte;        /* Pte entry del prossimo ram frame */
	swap_t* swap_entry;          /* Puntatore a swap_pool_table[next_frame_index] */     

	get_swap_pool_mutex();

	caller = &((sup->sup_exceptState)[PGFAULTEXCEPT]);
	entry_hi = caller->entry_hi;
	
	page_no = get_pageno(entry_hi);  /* proj_lib */
	
	page_index = ((page_no == UPROC_VIRT_STACK_PAGENO) ? 31:page_no);
	
#ifdef __PANDOS_DEBUGGER_ACTIVE__
	if (page_index > 31)
	{	
		debug_panic_loc = DEBUG_PAGER_ID + 4;
		debug_page_index = page_index;
		debug_page_no = page_no;

		PANIC();
	}
#endif

	next_frame_index = get_next_swap_index();
	io_val = 1;
	swap_entry = &(swap_pool_table[next_frame_index]);

	if (swap_entry->sw_asid != NOPROC)  /* Occupied */
	{
		/* ATOMIC */
		old_status = getSTATUS();
		setSTATUS(old_status & (~IECON) & (~TEBITON));
			
		page_pte = swap_entry->sw_pte;
		
		page_pte->pte_entryLO &= (~VALIDON);   /* Macro in pandos_const */
		
		invalid_tlb_entry(page_pte->pte_entryHI, page_pte->pte_entryHI, page_pte->pte_entryLO);

		/* FINE ATOMIC */
		setSTATUS(old_status);
		
		io_val = flash_io(swap_entry->sw_asid - 1, next_frame_index, swap_entry->sw_pageNo, FLASHWRITE);
	}

	if (io_val != 1)
	{
		release_swap_pool_mutex();
#ifdef __PANDOS_DEBUGGER_ACTIVE__
		debug_panic_loc = DEBUG_PAGER_ID + 11;
		debug_flash_status = io_val;
#endif
		PANIC();
		SYSCALL(TERMINATE, 0, 0, 0);
	}
	
	io_val = flash_io(sup->sup_asid - 1, next_frame_index, page_index, FLASHREAD);
	
	if (io_val != 1)
	{
		release_swap_pool_mutex();
#ifdef __PANDOS_DEBUGGER_ACTIVE__
		debug_panic_loc = DEBUG_PAGER_ID + 12;
		debug_flash_status = io_val;
#endif
		PANIC();
		SYSCALL(TERMINATE, 0, 0, 0);
	}
	
	swap_entry->sw_asid = sup->sup_asid;
	swap_entry->sw_pageNo = page_index;
	swap_entry->sw_pte = &((sup->sup_privatePgTbl)[page_index]);

	old_status = getSTATUS();
	setSTATUS(old_status & (~IECON) & (~TEBITON));
			
	(swap_entry->sw_pte)->pte_entryLO = __GET_FRAME(next_frame_index) | VALIDON | DIRTYON;

	invalid_tlb_entry((swap_entry->sw_pte)->pte_entryHI, (swap_entry->sw_pte)->pte_entryHI, (swap_entry->sw_pte)->pte_entryLO);

	setSTATUS(old_status);

	release_swap_pool_mutex();
	
	LDST(caller);
}

void mark_all_unoccupied(unsigned int asid)
{
	int i;

	for (i = 0; i < SWAPSIZE; i++)
	{
		if (swap_pool_table->sw_asid == asid)
		{
			(swap_pool_table[i]).sw_asid = NOPROC;
		}
	}
}


















