#include "pager.h"

#define SWAPSIZE 16

#ifdef __PANDOS_DEBUGGER_ACTIVE__

#include "debugger.h"

#endif

/* Funzioni Private */

static unsigned int swap_pool_h;           /* puntatore all'inizio della swap pool */    
static swap_t swap_pool_table[SWAPSIZE];   /* swap table */
static volatile int swap_pool_sem;         /* semaforo per la mutua esclusione sulla swap pool */

/* Ritorna l'indirizzo della pagina INDEX */
#define __GET_FRAME(INDEX) (swap_pool_h + ((INDEX) * PAGESIZE))


/* Wrapper per p(swap_pool_sem) */
static void get_swap_pool_mutex()
{
	SYSCALL(PASSEREN, (int) &swap_pool_sem, 0, 0);
}

/* Wrapper per v(swap_pool_sem) */
static void release_swap_pool_mutex()
{
	SYSCALL(VERHOGEN, (int) &swap_pool_sem, 0, 0);
}

/*
 * Page replacement algoritm, ritorna l'indice del prossimo frame della swap pool da usare.
 * Ritorna il primo frame libero, se presente, altrimenti round robin.
 * */
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

/*
 * flash_no : int compreso tra 0 e 7 che indichi su quale flash device si voglia
 *            lavorare.
 * frame_no : indice del frame della swap pool da leggere/scrivere
 * block_no : indice del blocco del flash da leggere/scrivere
 * command  : FLASHREAD per copiare dal flash alla swap poop, FLASHWRITE per copiare
 *            dalla swap pool al flash device
 *
 * return   : device status alla fine dell'operazione.
 *
 * Wrapper per le operazioni di lettura/scrittura sui device flash. Se il debugger di pandos e'
 * attivo controlla i parametri e fa PANIC in caso non siano validi.
 * */
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

	if ((frame_no < 0) || (frame_no > SWAPSIZE))
	{
		debug_panic_loc = DEBUG_PAGER_ID + 13;
		PANIC();
	}

	if (flash_no > 7)
	{
		debug_panic_loc = DEBUG_PAGER_ID + 14;
		PANIC();
	}
#endif

	unsigned int frame_ptr,    /* puntatore al frame della swap pool da leggere/scrivere */
				 old_status,   /* per fare la sezione atomica */
				 res;          /* device status al termine dell operazione */
	dtpreg_t* flas_reg_ptr;    /* puntatore al registro del flash */

	frame_ptr =  __GET_FRAME(frame_no);
	flas_reg_ptr = (dtpreg_t*) GET_DEVREG_ADDR(4, flash_no);
	
	SYSCALL(PASSEREN, (int) &(io_dev_mutex[FLASH_ROW][flash_no]), 0, 0);

	old_status = getSTATUS();
	setSTATUS(old_status & (~IECON) & (~TEBITON));
	
	flas_reg_ptr->data0 = frame_ptr;
	flas_reg_ptr->command = (block_no << 8) | command;
	res = SYSCALL(IOWAIT, 4, flash_no, 0);

	setSTATUS(old_status);
	
	SYSCALL(VERHOGEN, (int) &(io_dev_mutex[FLASH_ROW][flash_no]), 0, 0);

	return res;
}

/*
 * entry_hi     : EntryHi da cercare nella cache
 * new_entry_hi : NUovo valore di EntryHi da inserire nella cache se l'entry e' presente
 * new_entry_lo : Nuovo valore di EntryLo da inserire nella cache se l'entry e' presente
 *
 * Cerca (TLBP) nella cache una entry con il valore di entry_hi, se la trova la aggiorna (TLBWI) con i valori dati.
 * */
static void invalid_tlb_entry(unsigned int entry_hi, unsigned int new_entry_hi, unsigned int new_entry_lo)
{
	unsigned int old_entry_hi;
	unsigned int old_entry_lo;

	old_entry_hi = getENTRYHI();
	old_entry_lo = getENTRYLO();

	setENTRYHI(entry_hi);
	TLBP();

	if ((getINDEX() & 0x80000000) == 0)  /* Trovato */
	{
		setENTRYHI(new_entry_hi);
		setENTRYLO(new_entry_lo);

		TLBWI();
	}

	setENTRYHI(old_entry_hi);
	setENTRYLO(old_entry_lo);
}

/*
 * sup : puntatore al support struct del processo che ha provocato l'eccezione
 *
 * Ricava dal registro EntryHi di sup->sup_exceptState[PGFAULTEXCEPT] il vpn della 
 * pagina mancante, lo carica nel prossimo frame della swap pool (get_next_swap_index())
 * e aggiorna la cache e la tavola pte di sup. Se il prossimo frame e' gia' occupato il contenuto
 * viene salvato nel device flash del proprietario.
 *
 * Se il debugger e' attivo fa PANIC() se le operazioni sul flash falliscono o se il numero
 * della pagina e' mal formato (aka non e' una delle prime 31 pagine e non e' la pagina dello stack).
 * Se le operazioni sui flash falliscono e il debugger non e' attivo il processo viene terminato.
 * */
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
		PANIC();
#else
		SYSCALL(TERMINATE, 0, 0, 0);
#endif
	}
	
	io_val = flash_io(sup->sup_asid - 1, next_frame_index, page_index, FLASHREAD);
	
	if (io_val != 1)
	{
		release_swap_pool_mutex();
#ifdef __PANDOS_DEBUGGER_ACTIVE__
		debug_panic_loc = DEBUG_PAGER_ID + 12;
		debug_flash_status = io_val;
		PANIC();
#else
		SYSCALL(TERMINATE, 0, 0, 0);
#endif
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
			PANIC();   /* per debuggare, non dovrebbe succedere in questa fase */
#else
			SYSCALL(TERMINATE, 0, 0, 0);
#endif

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
	PANIC();  /* Should Not arrive here */

#endif
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

















