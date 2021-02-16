#include "pcb.h"

/* FUNZIONI PER ALLOCAZIONE/DEALLOCAZINE DI PCB */

/*
 * Free list, implementata come puntatore alla testa di
 * una lista monodirezionale di pcb (collegata tramite il
 * campo p_next).
 * */
static volatile pcb_PTR pcbFree_h;

/* Inserisce MAXPROC pcb nella free list, va chiamata una volta sola. */
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

/* Alloca un pcb inizializza tutti i campi a NULL o 0 e lo ritorna.
 * Se la pcb era vuota ritorna NULL */
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
	memset(&(res->p_s), 0, sizeof(state_t));
	memset(&(res->p_time), 0, sizeof(cpu_t));

	return res;
}

/* Ri inserisce il pcb dato nella free list */
void freePcb(pcb_PTR p)
{
	pcb_PTR sup = pcbFree_h;
	p->p_next = sup;
	pcbFree_h = p;
}


/* FUNZIONI PER LA GESTIONE DI CODE DI PCB */


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
  if(emptyProcQ(*tp)) return NULL;     /* Il caso in cui la coda e' vuota. */
  pcb_t *toBeRemoved = (*tp)->p_next;  /* L'inizializzazione di toBeRemoved che deve puntare a head */
  if(remove(toBeRemoved))              /* Gestisce il caso in cui la coda contiene solamente un elemento */
    *tp = NULL;
  return toBeRemoved;
}

/* Rimuove p dalla coda puntata da tp, aggiornando tp se necessario. 
 * Se p non e' presente nella coda ritorna NULL, altrimenti ritorna p. */
pcb_t* outProcQ(pcb_t **tp, pcb_t *p){
	
	if (emptyProcQ(*tp)) return NULL;
	
	/* Rispettivamente puntatore per scorrere la coda 
	 * e puntatore al pcb da eliminare */
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

pcb_PTR headProcQ(pcb_t* tp){
	if (tp == NULL) 
		return NULL;
	else
		return (tp)->p_next;
}

/* Inserisce p nella coda puntata da tp, 
 * se la coda era vuota p->p_next e p->p_prev punteranno a p,
 * in ogni caso *tp verra' aggiornato a p */
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


/* FUNZIONI PER LA GESTIONE DI ALBERI DI PCB */


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
