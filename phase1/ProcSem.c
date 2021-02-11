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

void initASL(void)
{
	static semd_t semdFree_table[MAXPROC + 2];
	int i;

	for (i = 0; i < MAXPROC; i++)
	{
		(semdFree_table + i)->s_next = semdFree_h;
		semdFree_h = semdFree_table + i;
	}

	(semdFree_table + MAXPROC)->s_semAdd = (int*) 0;
	(semdFree_table + MAXPROC)->s_next = semdFree_table + MAXPROC + 1;
	(semdFree_table + MAXPROC + 1)->s_semAdd = (int*) MAXINT;
	semd_h = semdFree_table + MAXPROC;
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
  semd_PTR tmp = searchAdd(semAdd);        /* Fa la ricerca del semaforo con la chiave semAdd */
  if(tmp != NULL){                        /* Questo if gestisce il caso in cui il semaforo e' stato trovato */
    insertProcQ(&(tmp->s_procQ), p);
    return 0;
  }
  if(semdFree_h == NULL) return 1;        /* Ritorna TRUE se la lista dei SEMD liberi o inutilizzati e' vuota */ 
  tmp = semdFree_h;                       /* Prende il primo elemento della semdFree per poi inserirlo nell'ASL */
  semdFree_h = semdFree_h->s_next;        /* Aggiorna il head di semdFree */
  semd_PTR sliding_tmp = semd_h;          /* Inizializza il puntatore al head dell'ASL neccessario per scorrere la lista 
					   * fino alla posizione giusta per inserire il nuovo elemento puntato dal semd */  
  while(sliding_tmp->s_next != NULL && sliding_tmp->s_next->s_semAdd > semAdd) sliding_tmp = sliding_tmp->s_next;
  tmp->s_next = sliding_tmp->s_next;      /* A questo punto si aggiunge tmp nell'ASL fra sliding_tmp e sliding_tmp->s_next */ 
  sliding_tmp->s_next = tmp;
  tmp->s_semAdd = semAdd;                 /* L'inizializzazione dei campi di tmp */
  tmp->s_procQ = mkEmptyProcQ();
  insertProcQ(&(tmp->s_procQ), p);
  return 0;
}

pcb_t* removeBlocked(int *semAdd)
{
	semd_t* s = searchAdd(semAdd);

	if (s == NULL)
	{
		return NULL;
	} else {
		pcb_t* res = removeProcQ(&(s->s_procQ));

		if (emptyProcQ(s->s_procQ))
		{
			semd_t* k = semd_h->s_next;

			while (k->s_next != s) k = k->s_next;

			k->s_next = s->s_next;

			s->s_next = semdFree_h;
			semdFree_h = s;
		}

		return res;
	}
}
