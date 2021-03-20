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
    memcpy(&(child->p_s), (state_t*) caller -> reg_a1, sizeof(state_t));

    if((state_t*) caller -> reg_a2 == NULL || caller -> reg_a2 == 0)
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

static void syscall2(state_t* caller)
{
    /* Stacca il padre */
    if (current_proc->p_prnt != NULL)
    {
        outChild(current_proc);
    }

    /* Itera fino all'ultimo figlio */
    pcb_t* child_queue = mkEmptyProcQ();

    insertProcQ(&child_queue, current_proc);
    pcb_t* k;

    while (!emptyProcQ(child_queue))
    {
        k = removeProcQ(&child_queue);

        while (!emptyChild(k))
        {
            insertProcQ(&child_queue, removeChild(k));
        }

        if (k->p_semAdd != NULL)
        {
            if (IS_DEV_SEMADDR(k->p_semAdd, dev_sem))
            {
                process_sb--;
            }
            else
            {
                process_b--;
            }

            outBlocked(k);
            (*(k->p_semAdd))++;
        }

        freePcb(k);
    }

    current_proc = NULL;
}

static void syscall3(state_t* caller){/* PASSEREN */
  int* semaddr = (int*) caller->reg_a1;
  (*semaddr)--;
  if((*semaddr) <= 0){
    insertBlocked(semaddr, current_proc);
    current_proc = NULL;
    if(IS_DEV_SEMADDR(semaddr, dev_sem))
      process_sb += 1;
    else
      process_b += 1;
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

		if (IS_DEV_SEMADDR(semaddr, dev_sem))
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
  if(intlNo < 7){
    dev_sem.sem_mat[intlNo-3][dnum] -= 1;
    insertBlocked(&dev_sem.sem_mat[intlNo-3][dnum], current_proc);
  }else{
    int termRead = caller->reg_a3;
    dev_sem.sem_mat[5-termread][dnum] -= 1;
    insertBlocked(&dev_sem.sem_mat[5-termread][dnum], current_proc);
  }
  current_proc = NULL;
  process_sb += 1;
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
    /* controllare che il dev relativo al clock sia dev_sem[0]*/
    cpu_t cur_time;

    STCK(cur_time);
    current_proc->p_time += (cur_time - tod_start);

    STCK(tod_start);
    SET_PC(*caller, getEPC()+4);
    memcpy(&(current_proc->p_s), caller, sizeof(state_t));
    dev_sem->sys_timer -= 1;
    insertBlocked(dev_sem, current_proc);
    current_proc = NULL;
    process_sb += 1;
}

static void syscall8(state_t* caller)
{
   /* Se current_proc->  */ 
    caller->reg_v0 = (unsigned int) current_proc->p_supportStruct;
}

void PassOrDie(state_t* caller, int exc_type)
{
    support_t* sup_puv; /* Support Level PassUp Vector */

    if ((sup_puv = current_proc->p_supportStruct) == NULL)
    {
        syscall2(caller);
    }
    else
    {
        /* Passup */
        state_t* excp_state = &(sup_puv->sup_exceptState[exc_type]);  
        context_t* sup_handler = &(sup_puv->sup_exceptContext[exc_type]);

        memcpy(excp_state, caller, sizeof(state_t));
        LDCXT(sup_handler->c_stackPtr, sup_handler->c_status, sup_handler->c_pc);
    }
}

void syscall_handler(state_t* caller){
  unsigned int a0 = caller->reg_v0;
  switch(a0){
    case 1:
      syscall1(caller);
      break;
    case 2:
      syscall2(caller);
      break;
    case 3:
      syscall3(caller);
      break;
    case 4:
      syscall4(caller);
      break;
    case 5:
      syscall5(caller);
      break;
    case 6:
      syscall6(caller);
      break;
    case 7:
      syscall7(caller);
      break;
    case 8:
      syscall8(caller);
      break;
    default:
      if(a0 >= 9)
	PassOrDie(caller, GENERALEXCEPT);
      break;
  }
}
