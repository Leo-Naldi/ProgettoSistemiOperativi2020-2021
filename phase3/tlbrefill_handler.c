#include "tlb_refill_handler.h"

void tlb_refill_handler()
{
	state_t* caller;          /* Saved exception state */
	unsigned int entry_hi;    /* Saved exception state's entryHi */ 
	int page_number;          /* Indice per scorrerere sup_privatePgTbl */
	support_t* cur_proc_sup;  /* Puntatore al p_supportStruct del current_process */
	pteEntry_t new_entry;     /* TLB entry da scrivere */

	caller = SAVED_STATE;
	entry_hi = caller->entry_hi;
	cur_proc_sup = current_process->p_supportStruct;
	page_number = 0;

	/* TODO C'e' un modo per trovare page number direttamente da entry_hi */
	while (((cur_proc_sup->sup_privatePgTbl)[page_number].pte_entryHI != entry_hi) &&
			(page_number < USERPGTBLSIZE))
	{
		page_number++;
	}

	if (page_number >= USERPGTBLSIZE) PANIC(); /* There was no matching tlb entry */

	new_entry = (cur_proc_sup->sup_privatePgTbl)[page_number];

	setENTRYHI(new_entry.pte_entryHI);
	setENTRYLO(new_entry.pte_entryLO);

	TLBWR();

	LDST(caller);
}
