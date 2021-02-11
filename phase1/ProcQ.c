#include "ProcQ.h"

/* Rimuove p dalla coda, facendo in modo che p->p_prev e p->p_next
 * si puntino a vicenda (tramite, rispettivamente, p_next e p_prev)
 * Ritorna 1 se p e' l'unico elemento della coda, 0 in ogni altro caso.
 * Undefined se p, p->p_next o p->p_prev sono NULL*/
static int remove(pcb_t *p)
{
	if (p->p_next == p) return 1;
	
	p->p_next->p_prev = p->p_prev;
	p->p_prev->p_next = p->p_next;

	return 0;
}

pcb_t* mkEmptyProcQ(){
  return NULL;
}

int emptyProcQ(pcb_t *tp){
  if(tp == NULL) return 1;
  return 0;
}

pcb_t* removeProcQ(pcb_t **tp){
  if(tp == NULL) return NULL;
  if(*tp == NULL) return NULL;
  pcb_t *toBeRemoved = (*tp)->p_next;
  remove(toBeRemoved);
  return toBeRemoved;
}

pcb_t* outProcQ(pcb_t **tp, pcb_t *p){
	
	if (emptyProcQ(*tp)) return NULL;
	
	/* Rispettivamente puntatore per scorrere la coda 
	 * e puntatore da eliminare */
	pcb_t* k = (*tp)->p_next;
	pcb_t* target = NULL;
	
	/* Se p e' l'ultimo si aggiorna tp*/
	if (p == *tp)
	{
		target = *tp;
		*tp = (*tp)->p_prev;
	}
	
	while ((target == NULL) && (k != *tp))
	{
		if (k == p) target = k;

		k = k->p_next;
	}

	if (target == NULL) return NULL;
	
	/* Se target era l'unico elemento, aggiorna tp*/
	if (remove(target)) *tp = NULL;

	return target;
}

pcb_PTR headProcQ(pcb_t** tp){
	if (*tp == NULL) 
		return NULL;
	else
		return (*tp)->p_next;
}

void insertProcQ(pcb_t **tp, pcb_t *p)
{
	if (emptyProcQ(*tp)) {
		*tp = p;
		p->p_prev = p;
		p->p_next = p;
	} else {
		p->p_next = (*tp)->p_next;
		(*tp)->p_next->p_prev = p;
		(*tp)->p_next = p;
		p->p_prev = *tp;
		*tp = p;
	}
}
