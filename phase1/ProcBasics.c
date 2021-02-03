#include "ProcBasics.h"

/*
 * Free list, implementata come puntatore alla testa di
 * una lista monodirezionale di pcb (collegata tramite il
 * campo p_next).
 * */
static volatile pcb_PTR pcbFree_h;


void initPcbs(void){
	
	static pcb_t pcbFree_table[MAXPROC];
	pcbFree_h = NULL;

	int i;
	for (i = MAXPROC - 1; i > -1; i--)
	{
		(pcbFree_table + i)->p_next = pcbFree_h;
		pcbFree_h = pcbFree_table + i;
	}
	
}

pcb_PTR allocPcb(void)
{
	if (pcbFree_h == NULL)
		return NULL;

	pcb_PTR res = pcbFree_h;
	pcbFree_h = pcbFree_h->p_next;
	res->p_next = NULL;

	return res;
}

void freePcb(pcb_PTR p)
{
}
