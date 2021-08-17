#include "tlbrefill_handler.h"

void tlb_refill_handler()
{
	state_t* caller;          /* Saved exception state */
	unsigned int entry_hi;    /* Saved exception state's entryHi */ 
	int page_number;          /* Numero della pagina fisica */
	support_t* cur_proc_sup;  /* Puntatore al p_supportStruct del current_process */
	pteEntry_t* new_entry;     /* TLB entry da scrivere */

	caller = SAVED_STATE;
	entry_hi = caller->entry_hi;
	cur_proc_sup = current_process->p_supportStruct;
	
	page_number = (entry_hi & GETPAGENO) >> VPNSHIFT;
	page_number = (page_no > 30) ? 31: page_number;
	
	new_entry = &((cur_proc_sup->sup_privatePgTbl)[page_number]);

	setENTRYHI(new_entry->pte_entryHI);
	setENTRYLO(new_entry->pte_entryLO);

	TLBWR();

	LDST(caller);
}
