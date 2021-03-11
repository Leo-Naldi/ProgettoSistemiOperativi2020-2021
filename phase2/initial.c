#include <umps/types.h>

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

#include "../phase1/pcb.h"
#include "../phase1/asl.h"

#include "scheduler.h"

extern void test();


int process_count;
int process_sb;
pcb_PTR ready_q;
pcb_PTR current_proc;

void initKer()
{

    passupvector_t* pu_vec0 = PASSUPVECTOR; /* lo struct di passupvec_t si trova in umps/types.h, la macro per PU_VEC0 si trova in fondo a pandos_const.h */
    pu_vec0 -> tlbrefllhandler =(memaddr) uTLBRefillHandler;
    pu_vec0 -> tlb_refill_stackPtr= exception_stackPtr = KERNELSTACK;
    pu_vec0 -> exceptionhandler = /*(memaddr) funzione creata da noi per gli exception handler */;

	initPcbs();
	initASL();

    process_count = 0;
    process_sb = 0;
    ready_q = mkEmptyProcQ();
    current_proc = NULL;

    *((unsigned int*)INTERVALTMR) = 100;

    pcb_PTR init = allocPcb();
    init -> p_time = 0;
    init -> p_semAdd = NULL;
    init -> supportStruct = NULL;

    /* setto ad ON (1) interrupt processor timer e kernel mode */
    init -> p_s = ALLOFF;
    inti -> p_s |= ON << 2;
    inti -> p_s |= OFF << 3;
    inti -> p_s |= ON << 27;

    // TODO mettere la funzione test nel program counter (non so dove sia)

}

int main()
{
	initKer();
	initScheduler();
	
	term_puts("done.\n");
	HALT();

	return 0;
}
