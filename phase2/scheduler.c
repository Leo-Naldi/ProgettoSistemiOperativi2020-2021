#include "scheduler.h"
#include "ker_exports.h"

extern int error_occurred;
extern int errcode;

void initScheduler() {}

void scheduler() 
{

	if ((current_proc = removeProcQ(&ready_q)) != NULL)
	{
		SET_PLT_ON(current_proc->p_s); /* Enable il PLT in caso non lo sia */
		setTIMER(TIMESLICE * (*((cpu_t *) TIMESCALEADDR))); /* set PLT. spero. */
		
		setSTATUS(getSTATUS() | TEBITON); /* Im like, 99% sure this is useless. */

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
		setSTATUS((getSTATUS() | IECON | IMON) & TEBITOFF);

		WAIT(); /* Calm */
	}
	else /* ready_q vuota, process_count > 0, process_sb == 0 */
	{

		if (process_count < 0)
		{
			term_puts("HAHAHAHAHAHA\n");
		}

		term_puts("Deadlock Detected\n");
		/* Panik */
		PANIC();
	}
}
