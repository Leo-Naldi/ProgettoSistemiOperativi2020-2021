#include "ProcSem.h"

/* Head pointer to single, linearly linked list of free semaphores*/
static volatile semd_PTR semdFree_h;

/* Head pointer to single, linearly linked
 * sorted list of Active Semaphores List
 * */
static volatile semd_PTR semd_h;

/*
 * Cerca nella semd_h un semaforo con s_semAdd uguale
 * a semAdd.
 *
 * E' fatta assumendo che sem_h sia sortata in senso crescente
 * in base a s_semAdd come da specifiche, e abbia due guardie
 * una all'inizio e una alla fine (quella finale con s_semAdd = MAXINT).
 * */
static semd_PTR searchAdd(int* semAdd)
{
	semd_PTR p = semd_h->s_next; /* skippa la guardia */

	while(p->s_semAdd != (int*) MAXINT)
	{
		if (p->s_semAdd == semAdd)
		{
			return p;
		}
		else if (p->s_semAdd > semAdd)
		{
			return NULL;
		}

		p = p->s_next;
	}


	return NULL;
}

pcb_PTR outBlocked(pcb_PTR p)
{
	semd_PTR s = searchAdd(p->p_semAdd);
	
	return outProcQ(&(s->s_procQ), p);
}

pcb_PTR headBlocked(int* semAdd)
{
	semd_PTR sp = searchAdd(semAdd);
	if(sp == NULL)
		return NULL;
	if(emptyProcQ(sp->s_procQ))
		return headProcQ(sp->s_procQ);
	else
		return NULL;
}

int insertBlocked(int *semAdd, pcb_t *p){
  semd_PTR tmp = semd_h;
  while(tmp != NULL && tmp->s_semAdd != semAdd) tmp = tmp->s_next;
  if(tmp != NULL){
    insertProcQ(&(tmp->s_procQ), p);
    return 0;
  }
  if(semdFree_h == NULL) return 1;
  tmp = semdFree_h;
  semdFree_h = semdFree_h->s_next;
  semd_PTR sliding_tmp = semd_h;
  while(sliding_tmp->s_next != NULL && *(sliding_tmp->s_next->s_semAdd) > *semAdd) sliding_tmp = sliding_tmp->s_next;
  tmp->s_next = sliding_tmp->s_next;
  sliding_tmp->s_next = tmp;
  tmp->s_semAdd = semAdd;
  tmp->s_procQ = mkEmptyProcQ();
  insertProcQ(&(tmp->s_procQ), p);
  return 0;
}
