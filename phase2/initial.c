#include <umps/types.h>
#include <umps/const.h>
#include <umps/libumps.h>

#include "pandos_const.h"
#include "pandos_types.h"
#include "proj_lib.h"

#include "pcb.h"
#include "asl.h"

#include "scheduler.h"

extern void test();
extern void uTLB_RefillHandler();

/* Per utilizzare queste variabili all interno di altri file basta includere ker_exports.h
 * (include anche tutti i moduli di fase 1, proj_lib e umps)*/

int process_count;
int process_sb;
int process_b;
pcb_t* ready_q;
pcb_PTR current_proc;

/* Probabilmente servono anche un'altro paio di variabili per il tempo di utilizzo della cpu */

int dev_sem[DEV_NO];  /* Semafori per i device, andrebbero definite delle macro per mappare gli
                         specifici device a indici di questi sem (in caso vanno messe in 
                         pandos_const sotto a DEV_NO). */

static void initKer()
{

    passupvector_t* pu_vec0 = (passupvector_t*) PASSUPVECTOR; /* lo struct di passupvec_t si trova in umps/types.h */
    pu_vec0 -> tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    pu_vec0 -> tlb_refill_stackPtr = pu_vec0->exception_stackPtr = KERNELSTACK;
    pu_vec0 -> exception_handler = 0/*(memaddr) funzione creata da noi per gli exception handler */;

	initPcbs();
	initASL();

    process_count = 0;
    process_sb = 0;
    process_b = 0;
    ready_q = mkEmptyProcQ();


    current_proc = allocPcb();
    current_proc -> p_time = 0;
    current_proc -> p_semAdd = NULL;
    current_proc -> p_supportStruct = NULL;
    
    STST(&(current_proc->p_s)); /* Not sure if this is needed o se basta una memset */

	RAMTOP(current_proc->p_s.reg_sp); /* Vedi pandos_const per la macro */
    current_proc->p_s.pc_epc = current_proc->p_s.reg_t9 = (unsigned int) test;
    
    /* macro definite in proj_lib.h */
    SET_KER_ON(current_proc->p_s);
    SET_ALL_INTERRUPT_ON(current_proc->p_s);

    insertProcQ(&ready_q, current_proc);
    current_proc = NULL;
    process_count++;

    /* setto ad ON (1) interrupt processor timer e kernel mode 
    init -> p_s = ALLOFF;
    init -> p_s |= ON << 2;
    init -> p_s |= OFF << 3;
    init -> p_s |= ON << 27;
    */


}

int main()
{
	initKer();
	initScheduler();
	

    /* TODO chiamare lo scheduler */

	term_puts("done.\n");
	HALT();

	return 0;
}
