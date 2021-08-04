#include "syscalls.h"
#include "ker_exports.h"



/* FUNZIONI PRIVATE */

static void syscall_solved(state_t* s)
{
    s->cause &= ~EXC_SYS;
}

/*
 * Procedura di ritorno per le syscall che bloccano il processo corrente.
 * Aggiorna il p_time, copia lo stato di eccezione nel p_s, setta il current_proc
 * e chiama lo scheduler.
 * */
static void ret_blocking(state_t* caller)
{ 
    update_cpu_usage(current_proc, &tod_start);
    memcpy(&(current_proc->p_s), caller, sizeof(state_t));
    
    syscall_solved(&(current_proc->p_s));

    current_proc = NULL;

    scheduler();
}

/*************************************************
 *
 * SYSCALL1: CREATETHREAD
 *
 * Crea un processo nuovo facendolo diventare figlio del processo corrente
 * inserisce il processo appena creato nella readyQ
 *
 *************************************************/
static void syscall1(state_t *caller)
{
    pcb_PTR child = allocPcb();

    /* Tutti i pcb disponibili sono stati allocati */
    if(child == NULL) {
        caller->reg_v0 = -1;
        update_cpu_usage(current_proc, &tod_start);
        LDST(caller);
    }
    memcpy(&(child->p_s), (state_t*) caller -> reg_a1, sizeof(state_t));
    
    child->p_supportStruct = NULL;

    /* se non mi viene fornita alcuna suppStruct allora diventa NULL */
    if( ((int*)(caller -> reg_a2) ==  NULL) || (caller->reg_a2 == 0))
        child -> p_supportStruct = NULL;
    else
        child -> p_supportStruct = (support_t*) caller -> reg_a2;

    insertProcQ(&ready_q, child);

    if (current_proc == NULL) PANIC();

    insertChild(current_proc, child);
    child -> p_time = 0;
    child -> p_semAdd = NULL;
    process_count += 1;
    caller -> reg_v0 = 0;
 
    syscall_solved(caller);
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
    if (current_proc == NULL) PANIC();    /* this should not be possible */

    pcb_t *b_queue = mkEmptyProcQ(), 
          *target,
          *child;

    outChild(current_proc);

    insertProcQ(&b_queue, current_proc);

    while ((target = removeProcQ(&b_queue)) != NULL)
    {
        while (!emptyChild(target))
        {
            child = removeChild(target);

            if (child->p_semAdd != NULL)
            {
              if (IS_DEV_SEMADDR(child->p_semAdd))
                process_sb--;
              else
                (*(child->p_semAdd))++;

              outBlocked(child);
            }
            else
            {
                outProcQ(&ready_q, child);
            }

            insertProcQ(&b_queue, child);
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

    if (insertBlocked(semaddr, current_proc)) PANIC();
    
    if (IS_DEV_SEMADDR(semaddr)) process_sb++;

    ret_blocking(caller);
  }

    syscall_solved(caller);
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
            
            if (IS_DEV_SEMADDR(semaddr)) process_sb--;

            p->p_semAdd = NULL;
            insertProcQ(&ready_q, p);
        }
    }

    syscall_solved(caller);
    update_cpu_usage(current_proc, &tod_start);
    LDST(caller);
	
}

/************************************************************
 *
 * SYSCALL5: WAIT FOR IO
 *
 * Blocca il current_proc sul semaforo del device corrispondente.
 * Il device e' identificato dalla linea di interrupt e dal device 
 * number (letti dai registri a1 e a2). Se la linea di interrupt e'
 * 7 (terminale), viene usato il registro a3 per determinare se 
 * e' un'operazione di read o no.
 *
 * Il current_proc viene ucciso (sys2) se i parametri sono sbagliati
 * (a1 deve essere compreso tra 3 e 7, a2 tra 0 e 7, se a2 e' 7 allora
 * a3 deve essere TRUE o FALSE).
 *
 ************************************************************/
static void syscall5(state_t* caller){
  int intlNo = caller->reg_a1;  /* interrupt line */
  int dnum = caller->reg_a2;    /* device number  */
  if(intlNo < 3 || intlNo > 7) syscall2(caller);
  if((dnum < 0) || (dnum > 7)) syscall2(caller);

  else if(intlNo < 7){
    dev_sem->sem_mat[intlNo-3][dnum] -= 1;
    if (insertBlocked(&dev_sem->sem_mat[intlNo-3][dnum], current_proc)) PANIC();
  }else{
    if ((caller->reg_a3 != TRUE) && (caller->reg_a3 != FALSE)) syscall2(caller);
    int termRead = caller->reg_a3;
    dev_sem->sem_mat[4+termRead][dnum] -= 1;
    if (insertBlocked(&dev_sem->sem_mat[4+termRead][dnum], current_proc)) PANIC();
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
    syscall_solved(caller);
  LDST(caller);
}


/****************************************************************
 *
 * SYSCALL7: WAIT FOR CLOCK
 *
 * inserisce il current proc in coda nel semaforo sys_timer
 * sys_timer è il device associato al clock
 *
 ****************************************************************/
static void syscall7(state_t *caller)
{
    dev_sem->sys_timer -= 1;
    if (insertBlocked(&(dev_sem->sys_timer), current_proc)) PANIC();
    process_sb += 1;
  
    ret_blocking(caller);
}

/******************************************************************
 *
 * SYSCALL8: GET SUPPORT DATA 
 *
 * Ritorna un puntatore al support struct del current_proc.
 *
 ******************************************************************/
static void syscall8(state_t* caller)
{
    caller->reg_v0 = (unsigned int) current_proc->p_supportStruct;
    syscall_solved(caller);
    update_cpu_usage(current_proc, &tod_start);
    LDST(caller);
}

/*******************************************************************
 *
 * Funzione per la gestione di program trap/syscall > 8.
 *
 * Il controllo viene passato alla struttura di supporto se presente,
 * altrimenti il current_proc viene ucciso.
 *
 *******************************************************************/
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
 * Utilizza il registro a0 per determinare quale syscall sia stata invocata.
 * Se il valore in a0 e' <= 0 il current_proc viene ucciso,
 * Se il current_proc era in user mode e a0 e' < 9 viene ucciso,
 * Se il valore in a0 e' >= 9 viene fatta una PassOrDie.
 *
 *****************************************************************************/
void syscall_handler(state_t* caller){
  unsigned int a0 = caller->reg_a0;

  if (a0 <= 0) syscall2(caller);
  
  if ((caller->status & STATUS_KUp_BIT) && (a0 < 9)) /* Process was running in user mode */
  {
      PassOrDie(caller, GENERALEXCEPT);
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
