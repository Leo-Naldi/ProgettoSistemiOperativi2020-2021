#include "syscalls.h"
#include "ker_exports.h"


int IS_DEV_SEMADDR(int* semaddr, dev_sem_list_t* d)
{
    if ((semaddr >= (int*) d) && (semaddr <= &(d->sem_mat[5][7])))
        return 1;
    else
        return 0;
}

/* FUNZIONI PRIVATE */

/*
 * Procedura di ritorno per le syscall che bloccano il processo corrente.
 * Aggiorna il p_time, copia lo stato di eccezione nel p_s, setta il current_proc
 * e chiama lo scheduler.
 * */
static void ret_blocking(state_t* caller)
{ 
    update_cpu_usage(current_proc, &tod_start);
    memcpy(&(current_proc->p_s), caller, sizeof(state_t));
    current_proc = NULL;

    scheduler();
}


static void syscall1(state_t *caller)
{
    /* NB in insertChild() il current proc è quello che ha chiamato la syscall (in teoria)*/
    pcb_PTR child = allocPcb();

    if (current_proc == NULL) PANIC();

    if(child == NULL) {
        caller->reg_v0 = -1;
        update_cpu_usage(current_proc, &tod_start);
        LDST(caller);
    }
    memcpy(&(child->p_s), (state_t*) caller -> reg_a1, sizeof(state_t));
    
    child->p_supportStruct = NULL;

    if( (caller -> reg_a2 ==  NULL) || (caller->reg_a2 == 0))
        child -> p_supportStruct = NULL;
    else
        child -> p_supportStruct = (support_t*) caller -> reg_a2;

    insertProcQ(&ready_q, child);
    insertChild(current_proc, child);
    child -> p_time = 0;
    child -> p_semAdd = NULL;
    process_count += 1;
    caller -> reg_v0 = 0;

    update_cpu_usage(current_proc, &tod_start);
    LDST(caller);
}

/*************************************************
 *
 * SYSCALL2: TERMINATETHREAD
 *
 * Uccide il processo correntemente in esecuzione e la sua progenie, 
 * ritorna il controllo allo scheduler.
 *
 *************************************************/
static void syscall2(state_t* caller)
{
    if (current_proc == NULL) PANIC();
    
    /* Stacca il padre */
    if (current_proc->p_prnt != NULL)
    {
        outChild(current_proc);
    }

    /* Coda dei pcb da uccidere */
    pcb_t* child_queue = mkEmptyProcQ();
    
    insertProcQ(&child_queue, current_proc);
    pcb_t* target, *child;

    while (!emptyProcQ(child_queue))
    {
        target = removeProcQ(&child_queue);
        
        /* Stacca tutti i figli di target, inserendoli nella child_queue,
         * e rimuovendoli dalla ready_q/sem su cui erano bloccati */
        while (!emptyChild(target))
        {
            child = removeChild(target);

            if (child->p_semAdd != NULL)
            {
                if (IS_DEV_SEMADDR(child->p_semAdd, dev_sem))
                {
                    process_sb--;
                }
                else
                {
                    process_b--;
                    ++(*(child->p_semAdd));
                }

                outBlocked(child);
            }
            else
            {
                outProcQ(&ready_q, child);
            }

            insertProcQ(&child_queue, child);
        }

        freePcb(target);
        process_count--;
    }


    current_proc = NULL;
    scheduler();
}


/***************************************************
 *
 * SYSCALL3: PASSEREN
 *
 * Esegue una P sul semaforo il cui indirizzo e' letto
 * da reg_a1. Se l'indirizzo letto e' NULL il 
 * current_proc viene ucciso. 
 *
 ***************************************************/
static void syscall3(state_t* caller){/* PASSEREN */
  int* semaddr = (int*) caller->reg_a1;
  
  if (semaddr == NULL) { syscall2(caller); }

  (*semaddr)--;
  if((*semaddr) < 0){

    insertBlocked(semaddr, current_proc);
    process_b++;

    ret_blocking(caller);
  }

  update_cpu_usage(current_proc, &tod_start);
  LDST(caller);
}

/***************************************************
 *
 * SYSCALL4: VERHOGEN 
 *
 * Esegue una V sul semaforo il cui indirizzo e' letto da
 * reg_a1, come in sys3 se l'indirizzo e' NULL il 
 * current_proc viene ucciso.
 *
 ***************************************************/
static void syscall4(state_t* caller) /* VERHOGEN */
{
	int* semaddr = (int*) caller->reg_a1;

    if (semaddr == NULL) { syscall2(caller); } 
	
	(*semaddr)++;
	
    if (*semaddr <= 0) /* Someone was waiting */
    {
        pcb_t* p = removeBlocked(semaddr);
        if (p != NULL)
        {
            /* NB : se cambia dev_sem deve cambiare anche sto if */
            
            process_b--;

            p->p_semAdd = NULL;
            insertProcQ(&ready_q, p);
        }
    }

    update_cpu_usage(current_proc, &tod_start);
    LDST(caller);
	
}

static void syscall5(state_t* caller){/* WAIT FOR IO DEVICE */
  int intlNo = caller->reg_a1;  /* interrupt line */
  int dnum = caller->reg_a2;    /* device number  */
  if(intlNo < 7){
    dev_sem->sem_mat[intlNo-3][dnum] -= 1;
    insertBlocked(&dev_sem->sem_mat[intlNo-3][dnum], current_proc);
  }else{
    int termRead = caller->reg_a3;
    dev_sem->sem_mat[4+termRead][dnum] -= 1;
    insertBlocked(&dev_sem->sem_mat[4+termRead][dnum], current_proc);
  }
  process_sb += 1;

  ret_blocking(caller);
}

/***************************************************************
 *
 * SYSCALL6: GET CPU TIME
 *
 * Ritorna in v0 il valore contenuto in current_proc->p_time.
 *
 ***************************************************************/
static void syscall6(state_t* caller) /* GET CPU TIME */
{
	/* In realta' current_proc e caller dovrebbero essere la stessa cosa */
    update_cpu_usage(current_proc, &tod_start);
	caller->reg_v0 = current_proc->p_time;
    LDST(caller);
}


/****************************************************************
 *
 * SYSCALL7: WAIT FOR CLOCK
 *
 ****************************************************************/
static void syscall7(state_t *caller)
{
    dev_sem->sys_timer -= 1;
    insertBlocked(&(dev_sem->sys_timer), current_proc);
    process_sb += 1;
  
    ret_blocking(caller);
}

static void syscall8(state_t* caller)
{
    caller->reg_v0 = (unsigned int) current_proc->p_supportStruct;
    
    update_cpu_usage(current_proc, &tod_start);
    LDST(caller);
}

void PassOrDie(state_t* caller, int exc_type)
{
    support_t* sup_puv = current_proc->p_supportStruct; /* Support Level PassUp Vector */

    if ((sup_puv == NULL))
    {
        syscall2(caller);
    }
    else
    {
        /* Passup */
        state_t* excp_state = &(sup_puv->sup_exceptState[exc_type]);  
        context_t* sup_handler = &(sup_puv->sup_exceptContext[exc_type]);

        memcpy(excp_state, caller, sizeof(state_t));
        
        update_cpu_usage(current_proc, &tod_start);
        LDCXT(sup_handler->c_stackPtr, sup_handler->c_status, sup_handler->c_pc);

    }
}

/*****************************************************************************
 *
 * Syscall handler.
 *
 *
 *
 *****************************************************************************/
void syscall_handler(state_t* caller){
  unsigned int a0 = caller->reg_a0;

  if (a0 <= 0) syscall2(caller);
  
  if ((caller->status & STATUS_KUp_BIT) && (a0 < 9)) /* Process was running in user mode */
  {
    /* Le slide dicono di far cosi, la guida di pandos dice di fare una
     * program trap. */
    syscall2(caller);
  }
  
  caller->pc_epc += 4;
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
      caller->pc_epc -= 4;
      PassOrDie(caller, GENERALEXCEPT);
      break;
  }
}
