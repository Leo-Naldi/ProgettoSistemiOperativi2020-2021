#ifndef PROCSEM_H
#define PROCSEM_H

#include "proj_lib.h"
#include "pandos_types.h"
#include "pandos_const.h"

#include "ProcQ.h"


void initASL(void);

/*
 * Rimuove p dalla lista dei processi bloccati del suo semaforo,
 * ritorna NULL se p non e' presente, esplode se il semaforo corrismpondente
 * a p_semAdd non e' presente nella lista.
 * */
pcb_PTR outBlocked(pcb_PTR p);

/*
 * Return the pointer at the head of the queue associated at semAdd
 * Return NULL if thw queue is empty or the semAdd doesn't exist
 */
pcb_PTR headBlocked(int* semAdd);

/*
 * Viene inserito il PCB puntato da p nella coda dei processi bloccati associata al SEMD con chiave
 * semAdd. Se il semaforo corrispondente non è presente nella ASL, alloca un nuovo SEMD dalla
 * lista di quelli liberi (semdFree) e lo inserisce nella ASL, settando I campi in maniera opportuna (i.e.
 * key e s_procQ). Se non è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota,
 * restituisce TRUE. In tutti gli altri casi, restituisce FALSE.
 */
int insertBlocked(int *semAdd, pcb_t *p);

pcb_t* removeBlocked(int *semAdd);

#endif
