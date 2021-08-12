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
  char *virtAddr = caller->reg_a1;
  int len = caller->reg_a2;
  if((virtAddr < KUSEG) || (virtAddr > MAXINT) || (len < 0) || (len > 128)){
    syscall9(caller);
  }
  
  unsigned int car_tr = 0;
  unsigned int devno = (((caller->s_entryHI) >> 6) - 1) % 8; // (ASID - 1) % 8
  devregtr *base = (devregtr*) GET_DEVREG_ADDR(PRNTINT, devno);
  devregtr status;

  SYSCALL(PASSERN, io_dev_mutex[PRINTER_ROW][devno], 0, 0);
  while(*virtAddr != EOS){
    *(base + 3) = PRINTCHR | (((devregtr) *virtAddr) << BYTELEN);
    status = SYSCALL(WAITIO, PRNTINT, devno, 0);
    if((status & 0xFF) != 1){ // controlla se device ready
      SYSCALL(VERHOGEN, io_dev_mutex[PRINTER_ROW][devno], 0, 0);
      caller->reg_v0 = -1 * status;
      return;
    }
    car_tr++;
    virtAddr++;
  }
  SYSCALL(VERHOGEN, io_dev_mutex[PRINTER_ROW][devno], 0, 0);
  caller->reg_v0 = car_tr;
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
  char *virtAddr = caller->reg_a1;
  int len = caller->reg_a2;
  if((virtAddr < KUSEG) || (virtAddr > MAXINT) || (len < 0) || (len > 128)){
    syscall9(caller);
  }

  unsigned int car_tr = 0;
  unsigned int devno = (((caller->s_entryHI) >> 6) - 1) % 8; // (ASID - 1) % 8
  devregtr *base = (devregtr*) GET_DEVREG_ADDR(TERMINT, devno);
  devregtr status;

  SYSCALL(PASSERN, io_dev_mutex[TERMW_ROW][devno], 0, 0);
  while(*virtAddr != EOS){
    *(base + 3) = PRINTCHR | (((devregtr) *virtAddr) << BYTELEN);
    status = SYSCALL(WAITIO, TERMINT, devno, 0);
    if((status & 0xFF) != 5){ // controlla se character transmitted
      SYSCALL(VERHOGEN, io_dev_mutex[TERMW_ROW][devno], 0, 0);
      caller->reg_v0 = -1 * status;
      return;
    }
    car_tr++;
    virtAddr++;
  }
  SYSCALL(VERHOGEN, io_dev_mutex[TERMW_ROW][devno], 0, 0);
  caller->reg_v0 = car_tr;
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
  char *virtAddr = caller->reg_a1;
  if((virtAddr < KUSEG) || (virtAddr > MAXINT)){
    syscall9(caller);
  }

  unsigned int car_tr = 0;
  unsigned int devno = (((caller->s_entryHI) >> 6) - 1) % 8; // (ASID - 1) % 8
  devregtr *base = (devregtr*) GET_DEVREG_ADDR(TERMINT, devno);
  devregtr status;

  SYSCALL(PASSERN, io_dev_mutex[TERMR_ROW][devno], 0, 0);
  while(*virtAddr != EOS){
    status = SYSCALL(WAITIO, TERMINT, devno, 1);
    if((status & 0xFF) != 5){ // controlla se character received
      SYSCALL(VERHOGEN, io_dev_mutex[TERMR_ROW][devno], 0, 0);
      caller->reg_v0 = -1 * status;
      return;
    }
    *virtAddr = (status >> 8);
    if(*virtAddr == EOS)
      break;
    car_tr++;
    virtAddr++;
  }
  SYSCALL(VERHOGEN, io_dev_mutex[TERMW_ROW][devno], 0, 0);
  caller->reg_v0 = car_tr;
}
