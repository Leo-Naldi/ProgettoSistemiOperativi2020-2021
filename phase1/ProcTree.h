#ifndef PROCTREE_H
#define PROCTREE_H

#include "pandos_types.h"
#include "pandos_const.h"
#include "proj_lib.h"


/*
 * Fa diventtare il pcb puntato da p un figlio del pcb puntato da prnt
 * 
 * i figli del padre sono una lista (Bidirezionale)
 */
void insertChild(pcb_PTR prnt, pcb_PTR p);

/*
 * Rimuove il primo figlio di p, ritorna NULL se p non ha figli. Undefined (probabilmente kernel_panic()) 
 * se p e' NULL. I figli del figlio rimosso vengono lasciati inalterati.
 *
 * E' fatto assumendo che la lista dei figli non sia circolare, se volete farla circolare per qualche motivo
 * lemme know che lo cambio.
 */
pcb_PTR removeChild(pcb_PTR p);

/*
 * Rimuove dalla lista dei figli il pcb puntato da p
 * 
 * Se il pcb è orfano (non ha padre) allora NULL
 */
 
pcb_PTR outChild(pcb_PTR p);
#endif
