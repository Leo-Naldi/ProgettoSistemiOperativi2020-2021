#include "ProcQ.h"

pcb_t* mkEmptyProcQ(){
  return NULL;
}

int emptyProcQ(pcb_t *tp){
  if(tp == NULL) return 1;
  return 0;
}

pcb_t* removeProcQ(pcb_t **tp){
  if(*tp == NULL) return NULL;
  pcb_t *toBeRemoved = (*tp)->p_prev;
  toBeRemoved->p_prev->p_next = *tp;
  (*tp)->p_prev = toBeRemoved->p_prev;
  return toBeRemoved;
}

