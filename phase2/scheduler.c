#include "scheduler.h"



void scheduler() 
{
	if(!emptyProcQ(ready_q))
	{
		current_proc = removeProcQ(&ready_q);

		setTIMER(TIMESLICE); /* set PLT. spero. */
		
		STCK(tod_start); /* Tempo in cui il processo ha iniziato a usare la cpu */

		LDST(&(current_proc->p_s));
	}
	else if (process_count == 0)
	{
		HALT(); /* Calm */
	}
	else if (process_sb > 0)
	{
		/* abilita gli interrupt */
		setSTATUS(ALLOFF | IECON | IMON);

		WAIT(); /* Calm */
	}
	else /* ready_q vuota, process_count > 0, process_sb == 0 */
	{
		/* Panik */
		PANIC();
	}
}

