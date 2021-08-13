/************************************************************************************
 *  SUPPORT LEVEL INIT MODULE
 ************************************************************************************
 * 
 * Copntiene le variabili globali e eventuale codice di inizializzazione, la roba 
 * esportabile sara' in sup_exports.
 *
 * Conterra il processo test
 *
 ************************************************************************************/

#include "proj_lib.h"
#include "pager.h"

int io_dev_mutex[6][DEVPERINT];  /* Semafori per la mutua esclusione sui device di IO */ 
								 /* Macro per le righe in sup_exports.h */

void init_uproc()
{
	int i, j;

	for (i = 0; i < 6; i++)
		for (j = 0; j < DEVPERINT; j++)
			io_dev_mutex[i][j] = 1;

	init_pager();
}
