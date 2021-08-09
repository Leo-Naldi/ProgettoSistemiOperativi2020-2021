#include "pager.h"

static sawp_t* swap_pool_h;          /* a casissimo */    
static swap_t swap_pool_table[16];   /* 2 * numero di proc massimo */
static volatile int swap_pool_sem;   /* semaforo per la mutua esclusione sulla swap pool */

static int get_next_swap_index();

static inline void get_swap_pool_mutex();

static inline void release_swap_pool_mutex();

static void pagefault_handler(support_t* sup);

void init_pager()
{
	swap_pool_h = SWAP_POOL_START;   /* Vedi proj_lib/pandos_const.h */
	swap_pool_sem = 1;
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
			get_swap_pool_mutex();
			
			pagefault_handler();

			release_swap_pool_mutex();
			break;
		default:
			PANIC();   /* chiamato l'handler sbagliato */
			break;
	}
	
	LDST(&((cur_proc_sup_ptr->sup_exceptState)[PGFAULTEXCEPT]));
}


static inline void get_swap_pool_mutex()
{
	SYSCALL(PASSEREN, (int) &swap_pool_sem, 0, 0);
}

static inline void release_swap_pool_mutex()
{
	SYSCALL(VERHOGEN, (int) &swap_pool_sem, 0, 0);
}

static void pagefault_handler(support_t* sup)
{
	int page_no,           /* Offending page number */
		entry_hi,          /* EntryHi reg di caller */
		next_frame_index;  /* Indice del prossimo ram frame */     
	state_t* caller;       /* Exception state */

	
	caller = SAVED_STATE;
	entry_hi = caller->entry_hi;
	page_no = entry_hi >> VPNSHIFT;
	next_frame_index = get_next_swap_index();

	if (swap_pool_table[next_frame_index].sw_asid != -1)  /* Occupied */
	{
		/* ATOMIC */
			/* Mark given swap pool table entry as invalid */
			/* After debugginf: controlla se la entry modificata
			 * era effettiamente in cache, if so correggila. 
			 * Per ora: butta via tutta la cache sempre */
		/* FINE ATOMIC */
	}
}





















