#include "interrupts.h"

/* Ritorna 0 se l'interrupt sul terminale dato
 * e' sul sub device in scrittura, uno se e' su quello
 * in lettura. 
 *
 * NB: non controlla se effettivamente ci sia
 * l'interrupt, se non lo trova sul sub-device di scrittura da per scontato
 * che sia su quello in lettura 
 */
int is_read(termreg_t* tr)
{
    /* Scrittura ha priorita' piu' alta
     * quindi controlla prima quella in caso 
     * entrambe siano accese */
    switch (tr->transm_status)
    {
        case 2: case 4: case 5:
            return 0;
        
        default:
            break;
    }

    return 1;
}


void interrupt_handler(state_t* caller)
{
    /*
     * Capire quale device ha mandato l'interrupt
     * Agire di conseguenza (si è un altro switch)
     *
     * */

    unsigned int cause = caller->cause;

    if (cause & LOCALTIMERINT)
    {

    /*
     * Qui scrivo la parte per i PLT Interrupt
     * Controllare se per setTIMER devo essere in kernel mode
     */
        setTIMER(TIMESLICE);
        memcpy(&(current_proc->p_s), caller, sizeof(state_t));
        insertProcQ(&ready_q, current_proc);
        current_proc = NULL;
    }
    else if (cause & TIMERINTERRUPT)
    {
        /* System wide interrupt handler */
        /* Caricare 100 mls */

        LDIT(PSECOND); 

        /* Liberare tutti i pcb */
        int* semadd =  &(dev_sem->sys_timer);
        
        pcb_t* proc = removeBlocked(semadd);

        while (proc != NULL)
        {
            insertProcQ(&ready_q, proc);
            process_sb--;

            proc = removeBlocked(semadd);
        }
        /* Resettare il sem */
        *semadd = 0;

    }
    else
    {
        /* Non-timer interrups */
    }
}
