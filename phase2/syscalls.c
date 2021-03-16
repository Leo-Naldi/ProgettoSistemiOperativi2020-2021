#include "syscalls.h"
#include "ker_exports.h"

/* Per ora le ho fatte di modo che prendano lo stato del chiamante
 * come parametro, che viene salvato in CP0 prima di tirare l'eccezione.
 * Sould be easy to get ma una cosa alla volta. */

static void syscall1(state_t *caller)
{
    /* NB in insertChild() il current proc è quello che ha chiamato la syscall (in teoria)*/
    pcb_PTR child = allocPcb();
    if(child == NULL) {
        caller->reg_v0 = -1;
        return;
    }
    memcpy(&(child->p_s), caller -> reg_a1, sizeof(state_t));

    if(caller -> reg_a2 == NULL || caller -> reg_a2 == 0)
        child -> p_supportStruct = NULL;
    else
        child -> p_supportStruct = (support_t*) caller -> reg_a2;

    insertProcQ(&ready_q, child);
    insertChild(current_proc, child);
    child -> p_time = 0;
    child -> p_semAdd = NULL;
    process_count += 1;
    caller -> reg_v0 = 0;
}

static void syscall3(state_t* caller){/* PASSEREN */
  int* semaddr = (int*) caller->reg_a1;
  if(headBlocked(semaddr) != NULL){
    insertBlocked(semaddr, current_proc);
    current_proc = NULL;
    process_sb += 1;
  } 
}

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

static void syscall5(state_t* caller){/* WAIT FOR IO DEVICE */
  int intlNo = caller->reg_a1;  /* interrupt line */
  int dnum = caller->reg_a2;    /* device number  */
  insertBlocked(dev_sem, current_proc);
  current_proc = NULL;
  process_sb += 1;
  /* ... */
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

static void syscall7(state_t *caller)
{
    // controllare che il dev relativo al clock sia dev_sem[0]
    cpu_t cur_time;

    STCK(cur_time);
    current_proc->p_time += (cur_time - tod_start);

    STCK(tod_start);
    SET_PC(*caller, getEPC()+4);
    memcpy(&(current_proc->p_s), caller, sizeof(state_t));
    dev_sem[0] -= 1;
    insertBlocked(dev_sem, current_proc);
    current_proc = NULL;
    process_sb += 1;
}

