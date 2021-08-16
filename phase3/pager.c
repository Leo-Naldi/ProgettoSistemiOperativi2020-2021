#include "pager.h"

#define SWAPSIZE 16

static int* swap_pool_h;                   /* puntatore all'inizio della swap pool */    
static swap_t swap_pool_table[SWAPSIZE];   /* swap table */
static volatile int swap_pool_sem;         /* semaforo per la mutua esclusione sulla swap pool */

static int get_next_swap_index();    /* ritorna l'indice del prossimo frame da usare */

static inline void get_swap_pool_mutex();      /* wrapper per p(swap_pool_sem) */

static inline void release_swap_pool_mutex();  /* wrapper per v(swap_pool_sem) */

static int flash_io(int flash_no, int frame_no, unsigned int block_no, int rw);  /* IO per i device flash 
																				  * (se rw e' 1 fa write else read) */

static void pagefault_handler(support_t* sup);  /* Pager vero e proprio */

void init_pager()
{
	swap_pool_h = SWAP_POOL_START;   /* Vedi proj_lib/pandos_const.h */
	swap_pool_sem = 1;
	
	int i;
	for (i = 0; i < SWAPSIZE; i++)
	{
		swap_pool_table[i].sw_asid = -1;
		swap_pool_table[i].sw_pageNo = -1;
		swap_pool_table[i].sw_pte = NULL;
	}
}

void tlb_exception_handler()
{
	support_t* cur_proc_sup_ptr;    /* puntatore alla struttura di supporto del current_process */
	unsigned int exception_code;    /* codice dell'eccezione */             

	cur_proc_sup_ptr = (support_t*) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
	exception_code = CAUSE_GET_EXCCODE((cur_proc_sup_ptr->sup_exceptState)[PGFAULTEXCEPT].cause);

	switch (exception_code)
	{
		case EXC_MOD:
			PANIC();   /* per debuggare, non dovrebbe succedere in questa fase */
			break;
		
		case EXC_TLBL:
		case EXC_TLBS:
			
			pagefault_handler(cur_proc_sup_ptr);

			break;
		
		default:
			PANIC();   /* chiamato l'handler sbagliato */
			break;
	}
	
	PANIC();  /* Should Not arrive here */
}


static inline void get_swap_pool_mutex()
{
	SYSCALL(PASSEREN, (int) &swap_pool_sem, 0, 0);
}

static inline void release_swap_pool_mutex()
{
	SYSCALL(VERHOGEN, (int) &swap_pool_sem, 0, 0);
}

static int get_next_swap_index()
{
	static int swap_index = 0;
	int res;

	res = swap_index;

	swap_index = (swap_index + 1) % SWAPSIZE;

	return res;
}

static int flash_io(int flash_no, int frame_no, unsigned int block_no, int rw)
{
	int* frame_ptr;            /* Pointer allo swap frame da usare */
	dtpreg_t* flash_reg;       /* Puntatore al registro del flash */
	int res;        		   /* return value, 0 se la read/write ha avuto successo, else 1 */
	unsigned int old_status;   /* Var ausiliaria per la sezione atomica */

	frame_ptr = swap_pool_h + frame_no * 1000;
	flash_reg = (dtpreg_t*) GET_DEVREG_ADDR(4, flash_no);
	res = 0;
	rw = ((rw == 1) ? FLASHWRITE: FLASHREAD);   /* 1 -> write to flash, else read from flash */
	
	SYSCALL(PASSEREN, (int) &(io_dev_mutex[FLASH_ROW][flash_no]), 0, 0);

	flash_reg->data0 = (unsigned int) frame_ptr;
	
	old_status = getSTATUS();
	setSTATUS(old_status & (~IECON));
	
	flash_reg->command = ALLOFF | (block_no << 7) | ((unsigned int) rw);
	res = SYSCALL(IOWAIT, 4, flash_no, 0);

	setSTATUS(old_status);

	SYSCALL(VERHOGEN, (int) &(io_dev_mutex[FLASH_ROW][flash_no]), 0, 0);

	res = ((res == 1) 0: 1);

	return res;
}

static void pagefault_handler(support_t* sup)
{
	int page_no,             /* Physical page index */
		entry_hi,            /* EntryHi reg di caller */
		next_frame_index,    /* Indice del prossimo ram frame */ 
		io_error;            /* 1 se un'operazione di scrittura/lettura del flash e' fallita */
	
	state_t* caller;             /* Exception state */
	unsigned int old_status;     /* Usata per spegnere gli interrupt */
	pteEntry_t* page_pte;        /* Pte entry del prossimo ram frame */
	swap_t* swap_entry;          /* Puntatore a swap_pool_table[next_frame_index] */     

	get_swap_pool_mutex();

	caller = SAVED_STATE;
	entry_hi = caller->entry_hi;
	
	page_no = (entry_hi & GETPAGENO) >> VPNSHIFT;
	page_no = (page_no > 30) ? 31: page_no;
	

	next_frame_index = get_next_swap_index();
	io_error = 0;
	swap_entry = &(swap_pool_table[next_frame_index]);

	if (swap_entry->sw_asid != -1)  /* Occupied */
	{
		/* ATOMIC */
		old_status = getSTATUS();
		setSTATUS(old_status & (~IECON));
			
		page_pte = swap_entry->sw_pte;
		
		page_pte->pte_entryLO &= (~VALIDON);   /* Macro in pandos_const */
		
		TLBCLR();  /* Per ora buttiamo via la cache sempre */

		/* FINE ATOMIC */
		setSTATUS(old_status);
		
		io_error = flash_io(swap_entry->sw_asid - 1, next_frame_index, swap_entry->sw_pageNo, 1);
	}

	if (io_error)
	{
		release_swap_pool_mutex();
		SYSCALL(TERMINATE, 0, 0, 0);
	}
	
	io_error = flash_io(cur_proc_sup_ptr->sup_asid - 1, next_frame_index, page_no, 0);
	
	if (io_error)
	{
		release_swap_pool_mutex();
		SYSCALL(TERMINATE, 0, 0, 0);
	}
	
	swap_entry->sw_asid = sup->sup_asid;
	swap_entry->sw_pageNo = page_no;
	swap_entry->sw_pte = &((sup->sup_privatePgTbl)[page_no]);

	old_status = getSTATUS();
	setSTATUS(old_status & (~IECON));
			
	(swap_entry->sw_pte).pte_entryLO |= VALIDON;
	(swap_entry->sw_pte).pte_entryLO &= (~GETPAGENO);
	(swap_entry->sw_pte).pte_entryLO |= ((swap_pool_h + 1000 * next_frame_index) << VPNSHIFT)
	
	TLBCLR();
	setSTATUS(old_status);

	release_swap_pool_mutex();
	
	LDST(caller);
}





















