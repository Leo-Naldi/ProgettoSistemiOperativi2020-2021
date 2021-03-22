#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "ker_exports.h"

int is_read(termreg_t* tr);

void interrupt_handler(state_t* caller);

/* Ritorna l'indice della riga di dev_sem->sem_mat che contiene
 * il semaforo giusto. L'ho messa qua cos non sapevo dove metterla */
#define GET_SEM_ROW(LINE_NO, DEV_NO) (((LINE_NO) != 7) ? ((LINE_NO) - 3) : ((LINE_NO) - 3 + is_read(LINE_NO, DEV_NO)))

#endif
