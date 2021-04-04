#ifndef KER_EXPORTS_H
#define KER_EXPORTS_H


/**********************************************************************************************************
 * 
 * File contenente gli export delle variabili globali del kernel (vedi initial.c per i dettagli)
 * e le funzioni di utility che utilizzano queste variabili.
 *
 * **********************************************************************************************************/

#include <umps/types.h>
#include <umps/const.h>
#include <umps/libumps.h>
#include <umps/cp0.h>

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

#include "pcb.h"
#include "asl.h"

#include "scheduler.h"
#include "syscalls.h"
#include "interrupts.h"

extern int process_count;
extern int process_sb;
extern int process_b;
extern pcb_t* ready_q;
extern pcb_PTR current_proc;

extern cpu_t tod_start;

extern dev_sem_list_t* dev_sem;

extern void print(char* str); /* Vedi p2test.c */

/* Ritorna 1 se semaddr e' l'indirizzo del semaforo di un device,
 * 0 altrimenti. */
extern int IS_DEV_SEMADDR(int* semaddr, dev_sem_list_t* ds);

#endif
