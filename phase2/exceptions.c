#include "exceptions.h"

int error_occurred = 0;
int errcode = 0;


/*
 * Exception handler, si occupa di richiamare gli handler relativi all'eccezione sollevata
 *
 * */
void exceHandler(){

    state_t* caller = SAVED_STATE;

    if (current_proc != NULL) update_cpu_usage(current_proc, &tod_start);
    
    switch (CAUSE_GET_EXCCODE(caller->cause)) {
        case EXC_INT:
            interrupt_handler(caller);
            break;
        case EXC_MOD: 
            PassOrDie(caller, PGFAULTEXCEPT);
            break;
        case EXC_TLBL: case EXC_TLBS:
            PassOrDie(caller, PGFAULTEXCEPT);
            break;
        case EXC_SYS:
            syscall_handler(caller);
            break;
        case EXC_ADEL: case EXC_ADES: case EXC_IBE: case EXC_DBE:
            case EXC_BP: case EXC_RI: case EXC_CPU: case EXC_OV:
            
            PassOrDie(caller, GENERALEXCEPT);
            break;
        }

}
