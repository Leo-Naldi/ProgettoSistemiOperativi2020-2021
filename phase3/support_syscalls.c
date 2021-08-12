/* FUNZIONI PRIVATE */

/*************************************************
 *
 * SYSCALL9: TERMINATE
 *
 * Un semplice wrapper per la system call corrispondente del kernel.
 *
 *************************************************/

static void syscall9(state_t *caller){
  syscall2(caller);
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
  /* da completare */
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
  /* da completare */
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
  /* da completare */
}
