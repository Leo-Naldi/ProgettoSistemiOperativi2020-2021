#ifndef SCHEDULER_H
#define SCEDULER_H

#include "ker_exports.h"

/* Scheduler di sistema, implementa un algoritmo round-robin, 
 * va' in HALT quando non ci sono piu' processi da eseguire,
 * PANIC se viene rilevato deadlock (sono presenti solo processi
 * bloccati).
 * */
void scheduler();

#endif
