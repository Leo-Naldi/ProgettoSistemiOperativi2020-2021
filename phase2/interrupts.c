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
    else{
      /* Non-timer interrupts */
      unsigned int IP = (cause >> 7) & 255;
      unsigned int intln = 0, tmp = IP;
      while(!(tmp%2)){
	if(tmp == 0) return;
	tmp = tmp >> 1;
	intln += 1;
      }
      unsigned int *devword = (unsigned int*)(0x1000.0040 + 0x04 * (intln - 3));
      unsigned int devno = 0, tmp = *devword;
      unsigned int tran = 0;
      /* da aggiungere precedenza di termread */
      if(intln != 7){
	while(!(tmp%2)){
	  if(tmp == 0) return;
	  tmp = tmp >> 1;
	  devno += 1;
	}
      }else{
	unsigned int devno_recv = 100;
	while(tmp!=0){
	  if(tmp%2){
	    unsigned int *devAddrBase = 0x1000.0054 + ((intln - 3) * 0x80) + (devno * 0x10);
	    unsigned int reg_status_recv = *devAddrBase;
	    unsigned int reg_status_tran = *(devAddrBase + 0x8);
	    if((reg_status_tran & 255) == 5){ tran = 1; break;}
	    else if(devno_recv == 100) devno_recv = devno;
	  }
	  tmp = tmp >> 1;
	  devno += 1;
	}
	if(!tran) devno = devno_recv;
      }
      unsigned int status = caller->status;
      unsigned int IEc = status % 2, IM = (status>>(7+intln)) % 2;
      if(!(IEc & IM)) return;
      unsigned int *devAddrBase = 0x1000.0054 + ((intln - 3) * 0x80) + (devno * 0x10);
      unsigned int reg_status = *(devAddrBase + tran * 0x8);
      *(devAddrBase + 0x4) = ACK;
      /* V operation */
      int* semaddr = NULL;
      if(intln < 7) semaddr = &dev_sem.sem_mat[intln-3][devno];
      else semaddr = &dev_sem.sem_mat[5-tran][devno];
      pcb_t* p = removeBlocked(semaddr);
      (*semaddr)++;
      if(p != NULL){
	process_sb--;
	p->p_s.s_v0 = reg_status;
	insertProcQ(&ready_q, p);
      }
      /* LDST */
    }
}
