#include "interrupts.h"


/*
 * Handler degli Interrupt
 * */
void interrupt_handler(state_t* caller)
{

    unsigned int cause = getCAUSE();

    if (cause & LOCALTIMERINT)
    {

        memcpy(&(current_proc->p_s), caller, sizeof(state_t));
        insertProcQ(&ready_q, current_proc);
        current_proc = NULL;
        scheduler();
    }
    else if (cause & TIMERINTERRUPT)
    {
        /* System wide interrupt handler */
        
        /* Carica 100ms nel system timer (vale come
         * ack dell'Interrupt) */
        LDIT(PSECOND); 

        /* Liberare tutti i pcb */
        int* semadd =  &(dev_sem->sys_timer);
        
        pcb_t* proc = removeBlocked(semadd);

        while (proc != NULL)
        {
            proc->p_semAdd = NULL;
            insertProcQ(&ready_q, proc);
            process_sb--;

            proc = removeBlocked(semadd);
        }
        /* Resettare il sem */
        *semadd = 0;
        
        STCK(tod_start);

        if (current_proc != NULL)
          LDST(caller);
        else
          scheduler();
    }
    else{
      /* Non-timer interrupts */
      unsigned int IP = (cause >> 8) & 255;
      unsigned int intln = 0, tmp = IP;
      while(!(tmp%2)){
	if(tmp == 0) PANIC(); 
	tmp = tmp >> 1;
	intln += 1;
      }
      unsigned int *devword = (unsigned int*)GET_DEVWORD(intln);
      unsigned int devno = 0; tmp = *devword;
      unsigned int tran = 0;
      if(intln != 7){
	while(!(tmp%2)) {
	  if(tmp == 0) PANIC();
	  tmp = tmp >> 1;
	  devno += 1;
	}
      }else{
	unsigned int devno_recv = 100;
	while(tmp!=0){
	  if(tmp%2){
	    devreg_t *devr = (devreg_t*)GET_DEVREG_ADDR(intln, devno);
	    unsigned int reg_status_tran = devr->term.transm_status;
	    if((reg_status_tran & 255) == 5){ tran = 1; break;}
	    else if(devno_recv == 100) devno_recv = devno;
	  }
	  tmp = tmp >> 1;
	  devno += 1;
	}
	if(!tran) devno = devno_recv;
      }
      /* if(!(IEc & IM)) HALT();  Viene beccata sempre */
      devreg_t *devr = (devreg_t*)GET_DEVREG_ADDR(intln, devno);
      unsigned int reg_status = 0;
      if(intln == 7){
	if(tran){
	  reg_status = devr->term.transm_status;
	  devr->term.transm_command = ACK;
	}
	else{
	  reg_status = devr->term.recv_status;
	  devr->term.recv_command = ACK;
	}
      }else{
	reg_status = devr->dtp.status;
	devr->dtp.command = ACK;
      }
      /* V operation */
      int* semaddr = NULL;
      if(intln < 7) semaddr = &(dev_sem->sem_mat[intln-3][devno]);
      else semaddr = &(dev_sem->sem_mat[5-tran][devno]);
      pcb_t* p = removeBlocked(semaddr);
      (*semaddr)++;
      if(p != NULL){
	process_sb--;
	p->p_s.reg_v0 = reg_status;
    p->p_semAdd = NULL;
	insertProcQ(&ready_q, p);
      }
      STCK(tod_start);
      if (current_proc != NULL)
	LDST(caller);
      else
	scheduler();
    }
}
