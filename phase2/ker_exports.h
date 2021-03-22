#ifndef KER_EXPORTS_H
#define KER_EXPORTS_H

#include <umps/types.h>
#include <umps/const.h>
#include <umps/libumps.h>
#include <umps/cp0.h>

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

#include "pcb.h"
#include "asl.h"

extern int process_count;
extern int process_sb;
extern int process_b;
extern pcb_t* ready_q;
extern pcb_PTR current_proc;

extern cpu_t tod_start;

#define DEV_SEM_CNT sizeof(dev_sem_list_t)

/* Semafori per device */
extern dev_sem_list_t* dev_sem;

/* Fine Macro indici */


#endif
