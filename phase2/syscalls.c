#include "syscalls.h"
#include "ker_exports.h"

/* Per ora le ho fatte di modo che prendano lo stato del chiamante
 * come parametro, che viene salvato in CP0 prima di tirare l'eccezione.
 * Sould be easy to get ma una cosa alla volta. */

static void syscall4(state_t* caller) /* VERHOGEN */
{
	int* semaddr = (int*) caller->reg_a1;
	
	pcb_t* p = removeBlocked(semaddr);

	(*semaddr)++;

	if (p != NULL)
	{
		/* NB : se cambia dev_sem deve cambiare anche sto if */

		if ((semaddr >= dev_sem) && (semaddr < dev_sem + DEVICECNT))
		{
			/* era il semaforo di un device */
			process_sb--;
		}
		else
		{
			process_b--;
		}

		insertProcQ(&ready_q, p);
	}
	
}

static void syscall6(state_t* caller) /* GET CPU TIME */
{
	/* In realta' current_proc e caller dovrebbero essere la stessa cosa */
	cpu_t cur_time;

	STCK(cur_time); /* Legge il Time-Of-Day Clock e lo scrive in cur_time */
	current_proc->p_time += (cur_time - tod_start);

	STCK(tod_start); /* Ogni volta che il current_proc->p_time viene aggiornato,
						Si deve anche resettare tod_start. Probabilemte sar' piu' comodo 
						aggiungere una funzione o qualcosa del genere */

	caller->reg_v0 = current_proc->p_time;	
}

