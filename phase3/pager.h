#ifndef _PAGER_H_
#define _PAGER_H_

#include "proj_lib.h"
#include "sup_exports.h"

/*
 * Inizializza le strutture dati del pager
 * */
void init_pager();

/*
 * Pagefault handler
 * */
void pager();

/*
 * Marca tutte le pagine occupate nella swap pool dal processo indicato
 * da asid come libere, usata in SYSCALL(TERMINATE)
 * */
void mark_all_unoccupied(unsigned int asid);

#endif
