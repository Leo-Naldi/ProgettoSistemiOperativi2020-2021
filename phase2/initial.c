#include "ker_exports.h"
#include "exceptions.h"

extern void test();
extern void tlb_refill_handler();

/* Per utilizzare queste variabili all interno di altri file basta includere ker_exports.h
 * (include anche tutti i moduli di fase 1, proj_lib e umps)*/

int process_count;  /* Contatore dei processi ancora vivi */
int process_sb;     /* Contatore dei processi bloccati su un device */
pcb_t* ready_q;     /* Coda dei processi pronti per entrare in esecuzione */
pcb_PTR current_proc;  /* Processo correntemente in esecuzione */


cpu_t tod_start; /* Variabile che conterra' il valore del TOD all-inizio dell'esecuzione
                    di un processo. */

/* Semafori dei device, vedi pandos_types per lo struct */
static dev_sem_list_t ds;
dev_sem_list_t* dev_sem;  /* Puntatore a ds */


void dummy();

/*  
 * Inizializza le variabili globali del kernel, il passupvector 0, i moduli di fase 1.
 * Crea inoltre il primo processo e lo inserisce nella ready_q.
 *  */
static void initKer()
{

    passupvector_t* pu_vec0 = (passupvector_t*) PASSUPVECTOR; /* lo struct di passupvec_t si trova in umps/types.h */
    pu_vec0 -> tlb_refill_handler = (memaddr) tlb_refill_handler;
    pu_vec0 -> tlb_refill_stackPtr = pu_vec0->exception_stackPtr = KERNELSTACK;
    pu_vec0 -> exception_handler = (memaddr) exceHandler; /*(memaddr) funzione creata da noi per gli exception handler */

	initPcbs();
	initASL();

    process_count = 0;
    process_sb = 0;
    ready_q = mkEmptyProcQ();


    current_proc = allocPcb();

    if (current_proc == NULL) 
        PANIC();

    current_proc -> p_time = 0;
    current_proc -> p_semAdd = NULL;
    current_proc -> p_supportStruct = NULL;
    
    current_proc->p_s.status = 0;
	RAMTOP(current_proc->p_s.reg_sp); /* Vedi pandos_const per la macro */
    SET_PC(current_proc->p_s, test);

    /* macro definite in proj_lib.h */
    SET_KER_ON(current_proc->p_s);
    SET_ALL_INTERRUPT_ON(current_proc->p_s);
    SET_PLT_ON(current_proc->p_s);

    insertProcQ(&ready_q, current_proc);
    current_proc = NULL;
    process_count++;
    
    dev_sem = &ds;
    
    int i, j;

    for (i = 0; i < 6; i++)
        for (j = 0; j < DEVPERINT; j ++)
            (dev_sem->sem_mat)[i][j] = 0;

    dev_sem->sys_timer = 0;

}

int main()
{
	initKer();	
 
    LDIT(PSECOND); /* Inizializza il system wide timer con 100ms */
   
    scheduler();

	PANIC(); /* Non si dovrebbe arrivare mai qui. */

	return 0;
}


void dummy()
{
    int i;
    while (i < 100) i++;
}
