#include "scheduler.h"


void scheduler() 
{
	if(!emptyProcQ(ready_q))  /* C'e' un processo pronto per essere eseguito */
	{
		current_proc = removeProcQ(&ready_q);

		setTIMER(TIMESLICE); /* set PLT. */
		
		STCK(tod_start); /* Tempo in cui il processo ha iniziato a usare la cpu */

		LDST(&(current_proc->p_s));
	}
	else if (process_count == 0)  /* Tutti i processi sono morti, we done. */
	{
		HALT();
	}
	else if (process_sb > 0)  /* C'e' almeno un processo soft-blocked. */
	{
		/* abilita gli interrupt, ALLOFF assicura che siamo in kernel mode con
		 * il plt disabilitato */
		setSTATUS(ALLOFF | IECON | IMON);

		WAIT(); 
	}
	else /* ready_q vuota, process_count > 0, process_sb == 0, deadlock */
	{
		PANIC();
	}
}

