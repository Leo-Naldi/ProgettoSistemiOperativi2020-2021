#ifndef PROCQ_H
#define PROCQ_H

#include "proj_lib.h"
#include "pandos_types.h"

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
pcb_PTR headProcQ(pcb_t** tp);

/* Inserisce l’elemento puntato da p nella coda dei processi tp. La doppia
 * indirezione su tp serve per poter inserire p come ultimo elemento della coda. */
void insertProcQ(pcb_t **tp, pcb_t *p);

#endif
