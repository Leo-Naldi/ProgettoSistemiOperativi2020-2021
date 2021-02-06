#ifndef PROCQ_H
#define PROCQ_H

#include "proj_lib.h"
#include "pandos_types.h"

pcb_t* mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

pcb_t* removeProcQ(pcb_t **tp);

pcb_t* outProcQ(pcb_t **tp, pcb_t *p);

#endif
