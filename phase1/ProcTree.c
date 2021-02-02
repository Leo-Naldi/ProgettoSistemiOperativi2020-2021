#include "ProcTree.h"

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
