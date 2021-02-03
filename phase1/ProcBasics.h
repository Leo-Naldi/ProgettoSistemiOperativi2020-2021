#ifndef PROCBASICS_H
#define PROCBASICS_H

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

/* Inizializza la free list, va chiamata una sola volta */
void initPcbs(void);

/* Alloca la memoria per un pcb rimuovendola dalla free list,
 * ritorna un puntatore alla memoria allocata, NULL se la 
 * free list era vuota.
 * */
pcb_PTR allocPcb(void);

void freePcb(pcb_PTR p);

#endif
