#ifndef PROCTREE_H
#define PROCTREE_H

#include "pandos_types.h"
#include "pandos_const.h"
#include "proj_lib.h"

/*
 * Rimuove il primo figlio di p, ritorna NULL se p non ha figli. Undefined (probabilmente kernel_panic()) 
 * se p e' NULL. I figli del figlio rimosso vengono lasciati inalterati.
 *
 * E' fatto assumendo che la lista dei figli non sia circolare, se volete farla circolare per qualche motivo
 * lemme know che lo cambio.
 */
pcb_PTR removeChild(pcb_PTR p);

#endif
