#include "asl.h"

/* Head pointer to single, linearly linked list of free semaphores*/
static volatile semd_PTR semdFree_h;

/* Puntatore alla asl, rappresentata come una lista singolarmente linkata
 * tramite il campo s_next e sortata tramite il campo s_semAdd. Ha due dummy
 * nodes, uno in testa e uno in fondo, rispettivamente con s_semAdd uguali a 0
 * e MAXINT.
 * */
static volatile semd_PTR semd_h;

/* Cerca il semaforo precedente a quello con s_semAdd uguale a 
 * semAdd nella asl e lo ritorna. Se un semaforo con un tale
 * s_semAdd non e' presente ritorna NULL.  */
static semd_PTR searchPrev(int* semAdd)
{
	semd_PTR p = semd_h;

	while(p->s_next->s_semAdd != (int*) MAXINT)
	{
		if (p->s_next->s_semAdd == semAdd)
		{
			return p;
		}
		else if (p->s_next->s_semAdd > semAdd)
		{
			return NULL;
		}

		p = p->s_next;
	}

	return NULL;
}

/*
 * Cerca nella asl un semaforo con s_semAdd uguale
 * a semAdd e lo ritorna. Se non presente ritorna 
 * NULL. 
 * */
static semd_PTR searchAdd(int* semAdd)
{
	semd_PTR p = searchPrev(semAdd); /* skippa la guardia */
	
	return (p == NULL) ? NULL: p->s_next;
}

void initASL(void)
{
	static semd_t semdFree_table[MAXPROC + 2];
	int i;

	semdFree_h = NULL;
	for (i = 0; i < MAXPROC; i++)
	{
		(semdFree_table + i)->s_next = semdFree_h;
		semdFree_h = semdFree_table + i;
	}
	(semdFree_table + MAXPROC)->s_semAdd = (int*) 0;
	(semdFree_table + MAXPROC)->s_next = (semd_t*) semdFree_table + MAXPROC + 1;
	(semdFree_table + MAXPROC + 1)->s_semAdd = (int*) MAXINT;
	(semdFree_table + MAXPROC + 1)->s_next = NULL;
	semd_h = (semd_t*) (semdFree_table + MAXPROC);
}

pcb_PTR outBlocked(pcb_PTR p)
{
	semd_PTR prev = searchPrev(p->p_semAdd);
	semd_PTR s = prev->s_next;
	pcb_PTR res;
	
	res = outProcQ(&(s->s_procQ), p);

	if (emptyProcQ(s->s_procQ))
	{
		prev->s_next = s->s_next;

		s->s_next = semdFree_h;
		semdFree_h = s;

		s->s_semAdd = NULL;
	}

	res->p_semAdd = NULL;
	
	return res; 
}

pcb_PTR headBlocked(int* semAdd)
{
	semd_PTR sp = searchAdd(semAdd); 	/* il semaforo non esiste */
	if(sp == NULL)
		return NULL;
	else if(emptyProcQ(sp->s_procQ))	/* la queue del semaforo è vuota */
		return NULL;
	else
		return headProcQ(sp->s_procQ);
}

int insertBlocked(int *semAdd, pcb_t *p){
  semd_PTR tmp = searchAdd(semAdd);       /* Fa la ricerca del semaforo con la chiave semAdd */
  if(tmp != NULL){                        /* Questo if gestisce il caso in cui il semaforo e' stato trovato */
    insertProcQ(&(tmp->s_procQ), p);
    p->p_semAdd = semAdd;
    return 0;
  }
  if(semdFree_h == NULL) return 1;        /* Ritorna TRUE se la lista dei SEMD liberi o inutilizzati e' vuota */ 
  tmp = semdFree_h;                       /* Prende il primo elemento della semdFree per poi inserirlo nell'ASL */
  semdFree_h = semdFree_h->s_next;          /* Aggiorna il head di semdFree */
  semd_PTR sliding_tmp = semd_h;          /* Inizializza il puntatore al head dell'ASL neccessario per scorrere la lista 
					   * fino alla posizione giusta per inserire il nuovo elemento puntato dal semd */  
  while(sliding_tmp->s_next->s_semAdd < semAdd) sliding_tmp = sliding_tmp->s_next;
  tmp->s_next = sliding_tmp->s_next;      /* A questo punto si aggiunge tmp nell'ASL fra sliding_tmp e sliding_tmp->s_next */ 
  sliding_tmp->s_next = tmp;
  tmp->s_semAdd = semAdd;                 /* L'inizializzazione dei campi di tmp */
  p->p_semAdd = semAdd;
  tmp->s_procQ = mkEmptyProcQ();
  insertProcQ(&(tmp->s_procQ), p);


  return 0;
}

/* Chiama removeProcQ sulla coda dei processi bloccati
 * del semaforo con s_semAdd uguale a semAdd.
 * Se un tale semaforo non dovesse esistere o se la coda
 * dovesse essere vuota ritorna NULL. Se la coda di processi
 * bloccati risulta vuota dopo la chiamata di removeProcQ il
 * semaforo viene rimosso dalla asl e reinserito nella Free List.
 *
 * Viene usata searchPrev al posto di searchAdd per evitare di dover
 * scorrere la asl due volte nel caso in cui il semaforo vada
 * elimitato */
pcb_t* removeBlocked(int *semAdd)
{
	semd_t* s = searchPrev(semAdd);

	if (s == NULL)
	{
		return NULL;
	}
	semd_t* k = s->s_next;
	pcb_t* res = removeProcQ(&(k->s_procQ));

	if (emptyProcQ(k->s_procQ))
	{
		s->s_next = k->s_next;

		k->s_next = semdFree_h;
		semdFree_h = k;

		k->s_semAdd = NULL;
	}

	res->p_semAdd = NULL;

	return res;	
}



































