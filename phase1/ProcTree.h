#ifndef PROCTREE_H
#define PROCTREE_H

#include "pandos_types.h"
#include "pandos_const.h"
#include "proj_lib.h"

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
