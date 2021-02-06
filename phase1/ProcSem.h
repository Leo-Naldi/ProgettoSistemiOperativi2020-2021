#ifndef PROCSEM_H
#define PROCSEM_H

#include "proj_lib.h"
#include "pandos_types.h"
#include "pandos_const.h"

#include "ProcQ.h"

/*
 * Rimuove p dalla lista dei processi bloccati del suo semaforo,
 * ritorna NULL se p non e' presente, esplode se il semaforo corrismpondente
 * a p_semAdd non e' presente nella lista.
 * */
pcb_PTR outBlocked(pcb_PTR p);

#endif
