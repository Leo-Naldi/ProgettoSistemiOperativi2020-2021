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
	for (i = 0; i < MAXPROC; i++)
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
	res->p_prev = NULL;
	res->p_prnt = NULL;
	res->p_child = NULL;
	res->p_next_sib = NULL;
	res->p_prev_sib = NULL;
	res->p_semAdd = NULL;

	return res;
}

void freePcb(pcb_PTR p)
{
	
	if(!pcbFree_h){
		pcbFree_h = p;
		p->p_next = NULL;
	}
	else {
		pcb_PTR sup = pcbFree_h;
		p->p_next = sup;
		pcbFree_h = p;
	}
}
