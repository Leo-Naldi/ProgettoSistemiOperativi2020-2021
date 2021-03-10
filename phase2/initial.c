#include <umps/types.h>

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

#include "pcb.h"
#include "asl.h"

#include "scheduler.h"


passupvector_t pu_vec0 = PU_VEC0; /* lo struct di passupvec_t si trova in umps/types.h, la macro per PU_VEC0 si trova in fondo a pandos_const.h */

void initKer()
{
	/* TODO: scrivere negli indirizzi in pu_vec0 quando avremo i vari handler, inizializzare
	 * il system wide timer e cose tipo il PC (credo) */

	initPcbs();
	initASL();
}

int main()
{
	initKer();
	initScheduler();
	
	term_puts("done.\n");
	HALT();

	return 0;
}
