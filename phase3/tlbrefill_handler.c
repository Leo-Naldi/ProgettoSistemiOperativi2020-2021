#include "tlbrefill_handler.h"

#ifdef __PANDOS_DEBUGGER_ACTIVE__

#include "debugger.h"

#endif

void tlb_refill_handler()
{
	state_t* caller;          /* Saved exception state */
	unsigned int entry_hi;    /* Saved exception state's entryHi */ 
	unsigned int page_number,          /* Numero della pagina fisica */
		  		page_index;
	support_t* cur_proc_sup;  /* Puntatore al p_supportStruct del current_process */
	pteEntry_t* new_entry;     /* TLB entry da scrivere */

	caller = SAVED_STATE;
	entry_hi = caller->entry_hi;
	cur_proc_sup = current_proc->p_supportStruct;
	
	page_number = (entry_hi & GETPAGENO);

	switch (page_number)
	{
		case (0xBFFFF000 & GETPAGENO):
			
			page_index = 31;
			break;
		
		default:
			page_index = (page_number & (~0x80000000)) >> VPNSHIFT;
			break;
	}
	
	if ((page_index < 0) || (page_index > 31))
	{
#ifdef __PANDOS_DEBUGGER_ACTIVE__

		debug_panic_loc = 5;
		debug_page_index = page_index;
		debug_page_no = page_number;
		debug_entry_hi = entry_hi;
#endif
		PANIC();
	}

	new_entry = &((cur_proc_sup->sup_privatePgTbl)[page_index]);

	setENTRYHI(new_entry->pte_entryHI);
	setENTRYLO(new_entry->pte_entryLO);

	TLBWR();
	

	LDST(caller);
}
