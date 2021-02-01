#ifndef PROCBASICS_H
#define PROCBASICS_H

#include "pandos_const.h"
#include "pandos_types.h"


extern pcb_PTR pcbFree_h;
extern pcb_t pcbFree_table[MAXPROC];

extern void initPcbs(void);

extern pcb_PTR allocPcb(void);

extern void freePcb(pcb_PTR p);

#endif
