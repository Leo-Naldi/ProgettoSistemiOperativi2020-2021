#include "support_syscalls.h"

typedef unsigned int devregtr;

/* FUNZIONI PRIVATE */

/*************************************************
 *
 * SYSCALL9: TERMINATE
 *
 * Un semplice wrapper per la system call corrispondente del kernel.
 *
 *************************************************/

static void syscall9(state_t *caller){
 
  mark_all_unoccupied(get_asid(caller->entry_hi));

  SYSCALL(VERHOGEN, (int) &master_sem, 0, 0);
  SYSCALL(TERMINATEPROCESS, 0, 0, 0);
}

/*************************************************
 *
 * SYSCALL10: GET_TOD
 *
 * Restituisce il numero di microsecondi passati dall’accensione del sistema.
 *
 *************************************************/

static void syscall10(state_t *caller){
  cpu_t tod;
  STCK(tod);
  caller->reg_v0 = tod;
  LDST(caller);
}

/*************************************************
 *
 * SYSCALL11: WRITEPRINTER
 *
 * Richede una stampa ininterrotta della stringa richiesta sulla stampante associata al processo.
 * Se l’indirizzo e’ fuori dalla memoria virtuale del processo o la lunghezza richiesta e’ zero deve
 * risultare nella sua terminazione. Restituisce il numero di caratteri stampati in caso di successo, 
 * l’opposto dello stato del dispositivo in caso contrario.
 *
 *************************************************/

static void syscall11(state_t *caller){
  char *virtAddr = (char*) caller->reg_a1;
  int len = caller->reg_a2;

  unsigned int old_status;

  if( (((unsigned int) virtAddr) < KUSEG) || (len < 0) || (len > 128)) syscall9(caller); /* indirizzo out of range */
  unsigned int car_tr = 0; /* caratteri trasmessi */
  unsigned int devno = get_asid(caller->entry_hi) - 1; /* (ASID - 1) */
  dtpreg_t *printer = (dtpreg_t*) GET_DEVREG_ADDR(PRNTINT, devno);
  devregtr status;

  SYSCALL(PASSEREN,(int) &(io_dev_mutex[PRINTER_ROW][devno]), 0, 0); /* mutua esclusione sul semaforo del device */
  
  while(car_tr < len){ /* trasmissione dei caratteri */
    
    old_status = getSTATUS();
    setSTATUS(old_status & (~IECON) & (~TEBITON));

    printer->command = PRINTCHR;
    printer->data0 = virtAddr[car_tr];
    status = SYSCALL(WAITIO, PRNTINT, devno, 0);
    
    setSTATUS(old_status);
    
    if((status & 0xFF) != 1){ /* controlla se device ready */
      SYSCALL(VERHOGEN, (int) &(io_dev_mutex[PRINTER_ROW][devno]), 0, 0);
      caller->reg_v0 = -1 * status;
      LDST(caller);
    }
    car_tr++;
  }
  SYSCALL(VERHOGEN, (int) &(io_dev_mutex[PRINTER_ROW][devno]), 0, 0); /* rilascio */
  caller->reg_v0 = car_tr;
  LDST(caller);
}

/*************************************************
 *
 * SYSCALL12: WRITETERMINAL
 *
 * Richede una stampa ininterrotta della stringa richiesta sul terminale associato al processo.
 * Se l’indirizzo e’ fuori dalla memoria virtuale del processo o la lunghezza richiesta e’ zero deve
 * risultare nella sua terminazione. Restituisce il numero di caratteri stampati in caso di successo, 
 * l’opposto dello stato del dispositivo in caso contrario.
 *
 *************************************************/

static void syscall12(state_t *caller){
  unsigned int old_status;

  char *virtAddr = (char*) caller->reg_a1;
  int len = caller->reg_a2;
  
  if( (((unsigned int) virtAddr) < KUSEG) || (len < 0) || (len > 128)) syscall9(caller); /* indirizzo out of range */
  unsigned int car_tr = 0; /* caratteri trasmessi */
  unsigned int devno = get_asid(caller->entry_hi) - 1; /* (ASID - 1) */
  termreg_t *base = (termreg_t*) GET_DEVREG_ADDR(7, devno);
  devregtr status;

  SYSCALL(PASSEREN, (int) &(io_dev_mutex[TERMW_ROW][devno]), 0, 0); /* mutua esclusione sul semaforo del device */
  while(car_tr < len){
    
    old_status = getSTATUS();
	setSTATUS(old_status & (~IECON) & (~TEBITON));
    
    base->transm_command = PRINTCHR | (((devregtr) (virtAddr[car_tr])) << BYTELEN);
    status = SYSCALL(WAITIO, TERMINT, devno, 0);
    
    setSTATUS(old_status);
    
    if((status & 0xFF) != 5){ /* controlla se character transmitted */
      SYSCALL(VERHOGEN, (int) &(io_dev_mutex[TERMW_ROW][devno]), 0, 0);
      caller->reg_v0 = -1 * status;
      LDST(caller);
    }
    car_tr++;
  }
  SYSCALL(VERHOGEN, (int) &(io_dev_mutex[TERMW_ROW][devno]), 0, 0); /* rilascio */
  caller->reg_v0 = car_tr;
  LDST(caller);
}

/*************************************************
 *
 * SYSCALL13: READTERMINAL
 *
 * Legge una riga (fino al newline) dal terminale associato al processo. 
 * Se l’indirizzo e’ fuori dalla memoria virtuale del processo o la lunghezza richiesta e’ zero deve
 * risultare nella sua terminazione. Restituisce il numero di caratteri stampati in caso di successo, 
 * l’opposto dello stato del dispositivo in caso contrario.
 *
 *************************************************/

static void syscall13(state_t *caller){
  char *virtAddr = (char*) caller->reg_a1;
  char rechvd_char;
  if( (((unsigned int)virtAddr) < KUSEG)) syscall9(caller);

  unsigned int old_status;

  unsigned int car_tr = 0; /* caratteri trasmessi */
  unsigned int devno = get_asid(caller->entry_hi) - 1; /* (ASID - 1) */
  devregtr *base = (devregtr*) GET_DEVREG_ADDR(TERMINT, devno);
  devregtr status;

  SYSCALL(PASSEREN, (int) &(io_dev_mutex[TERMR_ROW][devno]), 0, 0); /* mutua esclusione sul semaforo del device */
  do {
     
    old_status = getSTATUS();
	setSTATUS(old_status & (~IECON) & (~TEBITON));
    
    *(base + 1) = 2;  /* legge carattere */
    status = SYSCALL(WAITIO, TERMINT, devno, 1);
    
    setSTATUS(old_status);
    
    if((status & 0xFF) != 5){ /* controlla se character received */
      SYSCALL(VERHOGEN, (int) &(io_dev_mutex[TERMR_ROW][devno]), 0, 0);
      caller->reg_v0 = -1 * status;
      LDST(caller);
    }
    rechvd_char = ((status & 0x0000FF00) >> 8); 
    
    virtAddr[car_tr++] = rechvd_char;
  } while (rechvd_char != '\n');

  SYSCALL(VERHOGEN,(int) &(io_dev_mutex[TERMR_ROW][devno]), 0, 0); /* rilascio */
  caller->reg_v0 = car_tr;
  LDST(caller);
}

/*****************************************************************************
 *
 * Support level syscall handler.
 *
 * Utilizza il registro a0 per determinare quale syscall sia stata invocata.
 * Se il valore in a0 e' >= 14 il current_proc viene ucciso
 *
 *****************************************************************************/
void support_syscall_handler(state_t *caller){
  unsigned int a0 = caller->reg_a0;
  
  if (a0 <= 8 || a0 >= 14) SYSCALL(TERMINATE, 0, 0, 0);
  
  caller->pc_epc += 4;
  switch(a0){
    case 9:
      syscall9(caller);
      break;
    case 10:
      syscall10(caller);
      break;
    case 11:
      syscall11(caller);
      break;
    case 12:
      syscall12(caller);
      break;
    case 13:
      syscall13(caller);
      break;
  }
}
