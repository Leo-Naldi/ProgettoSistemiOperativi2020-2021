#include "ProcTree.h"

int emptyChild(pcb_t *p){
  if(p->p_child == NULL) return 1;
  return 0;
}

void insertChild(pcb_PTR prnt, pcb_PTR p)
{
	if(p == NULL)
		return;
	if(prnt->p_child == NULL){
		prnt->p_child =p;
		p->p_next_sib = NULL;
		p->p_prev_sib = NULL;
		p->p_prnt = prnt;
	} else {
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
 	pcb_PTR res;
 	if(p->p_next_sib != NULL && p->p_prev_sib != NULL){
 		res = p;
 		res->p_next_sib->p_prev_sib = res->p_prev_sib;
 		res->p_prev_sib->p_next_sib = res->p_next_sib;
 		res->p_prnt = NULL;
 		res->p_next_sib = NULL;
 		res->p_prev_sib = NULL;
 	} else if(p->p_prev_sib == NULL){
 		res = p;
 		res->p_prnt->p_child = res->p_next_sib;
 	 	res->p_prnt = NULL;
 	 	res->p_next_sib->p_prev_sib = NULL;
 	 	res->p_next_sib = NULL;
 	} else if (p->p_next_sib == NULL){
 		res = p;
 	 	res->p_prnt = NULL;
 		res->p_prev_sib->p_next_sib = NULL;
 		res->p_prev_sib = NULL;
 	} else {
 		res = p;
 		res->p_prnt->p_child = NULL;
 		res->p_prnt = NULL;
 	}
 	return res;
 }
