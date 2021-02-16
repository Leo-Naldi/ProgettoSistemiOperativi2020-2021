#ifndef PROCBASICS_H
#define PROCBASICS_H

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

/* FUNZIONI PER ALLOCAZIONE/DEALLOCAZIONE DI PCB */

/* Inizializza la free list, va chiamata una sola volta */
void initPcbs(void);

/* Alloca la memoria per un pcb rimuovendola dalla free list, 
 * inizializza tutti i campi a 0 o a NULL e
 * ritorna un puntatore alla memoria allocata, NULL se la 
 * free list era vuota.
 * */
pcb_PTR allocPcb(void);

/* Inserisce nella free list il pcb passatogli */
void freePcb(pcb_PTR p);


/* FUNZIONI PER LA GESTIONE DI CODE DI PCB */


/* Crea una lista di PCB, inizializzandola come lista vuota (i.e. restituisce NULL). */
pcb_t* mkEmptyProcQ();

/* Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti. */
int emptyProcQ(pcb_t *tp);

/* Rimuove l’elemento piu’ vecchio dalla coda tp. Ritorna NULL se la coda è vuota,
 * altrimenti ritorna il puntatore all’elemento rimosso dalla lista. */
pcb_t* removeProcQ(pcb_t **tp);

/* Rimuove il PCB puntato da p dalla coda dei processi puntata da tp. Se p non è presente
 * nella coda, restituisce NULL (p può trovarsi in una posizione arbitraria della coda). */
pcb_t* outProcQ(pcb_t **tp, pcb_t *p);

/* Restituisce l’elemento in fondo alla coda dei processi tp, SENZA RIMUOVERLO.
 * Ritorna NULL se la coda non ha elementi. */
pcb_PTR headProcQ(pcb_t* tp);

/* Inserisce l’elemento puntato da p nella coda dei processi tp. La doppia
 * indirezione su tp serve per poter inserire p come ultimo elemento della coda. */
void insertProcQ(pcb_t **tp, pcb_t *p);


/* FUNZIONI PER LAGESTIONE DI ALBERI DI PCB */


/* Restituisce TRUE se il PCB puntato da p non ha figli, FALSE altrimenti. */
int emptyChild(pcb_t *p);

/* Fa diventtare il pcb puntato da p un figlio del pcb puntato da prnt 
 * i figli del padre sono una lista (Bidirezionale) */
void insertChild(pcb_PTR prnt, pcb_PTR p);

/* Rimuove il primo figlio di p, ritorna NULL se p non ha figli. Undefined (probabilmente kernel_panic()) 
 * se p e' NULL. I figli del figlio rimosso vengono lasciati inalterati. */
pcb_PTR removeChild(pcb_PTR p);

/* Rimuove dalla lista dei figli il pcb puntato da p.
 * 
 * Se il pcb è orfano (non ha padre) allora NULL
 *
 *  casi possibili:
 *   figlio senza fratelli
 *   figlio con fratelli a dx
 *   figlio con fratelli a sx
 *   figlio con fratelli a dx e sx */
pcb_PTR outChild(pcb_PTR p);

#endif
