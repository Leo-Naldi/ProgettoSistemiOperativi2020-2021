#include "ProcQ.h"

pcb_t* mkEmptyProcQ(){
  return NULL;
}

int emptyProcQ(pcb_t *tp){
  if(tp == NULL) return 1;
  return 0;
}

pcb_t* removeProcQ(pcb_t **tp){
  if(tp == NULL) return NULL;
  if(*tp == NULL) return NULL;
  pcb_t *toBeRemoved = (*tp)->p_next;
  toBeRemoved->p_next->p_prev = *tp;
  (*tp)->p_next = toBeRemoved->p_next;
  return toBeRemoved;
}

pcb_t* outProcQ(pcb_t **tp, pcb_t *p){
	return NULL;
}

pcb_PTR headProcQ(pcb_PTR tp){
	return NULL;
}
