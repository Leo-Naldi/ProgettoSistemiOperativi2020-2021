#include <umps/libumps.h>

void interrupt_handler(state_t* caller)
{
    /*
     * Capire quale device ha mandato l'interrupt
     * Agire di conseguenza (si è un altro switch)
     *
     * */

    /*
     * Qui scrivo la parte per i PLT Interrupt
     * Controllare se per setTIMER devo essere in kernel mode
     *
     *  setTIMER(5);
     *  memcpy(&(current_proc->p_s), caller, sizeof(state_t));
     *  insertProcQ(ready_q, current_proc);
     *  scheduler();
     *
     * */
}