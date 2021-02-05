#include "ProcTree.h"

void insertChild(pcb_PTR prnt, pcb_PTR p)
{
	if(p == NULL)
		return;
	if(prnt->p_child == NULL){
		term_puts("siamo nel secondo if\n");
		prnt->p_child =p;
		p->p_next_sib = NULL;
		p->p_prev_sib = NULL;
		p->p_prnt = prnt;
	} else {
		term_puts("siamo nel else\n");
		prnt->p_child->p_prev_sib = p;	
		p->p_next_sib = prnt->p_child;
		p->p_prev_sib = NULL;
		p->p_prnt = prnt;
		prnt->p_child = p;
	}
}

pcb_PTR removeChild(pcb_PTR p)
{
	if (p->p_child == NULL)
		return NULL;

	pcb_PTR target = p->p_child;
	p->p_child = (p->p_child)->p_next_sib;

	if (p->p_child != NULL)
		(p->p_child)->p_prev_sib = NULL;

	target->p_prnt = NULL;
	target->p_next_sib = NULL;

	return target;
}

pcb_PTR outChild(pcb_PTR p)
 {	
 	if(p->p_prnt == NULL)
 		return NULL;
 	pcb_PTR res = p;
 	p->p_prnt = NULL;
 	p->p_next_sib->p_prev_sib = p->p_prev_sib;
 	p->p_prev_sib->p_next_sib = p->p_next_sib;
 	return res;
 }
