#include <umps/libumps.h>

void exceHandler(){
    state_t* caller = SAVED_STATE;
    switch (CAUSE_GET_EXCCODE(getCAUSE())) {
        case EXC_INT:
            interrupt_handler(caller);
            break;
        case EXC_MOD: case EXC_TLBL: case EXC_TLBS:
            PassOrDie(caller, PGFAULTEXCEPT)
            break;
        case EXC_SYS:
            syscall_handler(caller);
            break;
        case EXC_ADEL: case EXC_ADES: case EXC_IDE: case EXC_DBE:
            case EXC_BP: case EXC_RI: case EXC_CPU: case EXC_OV:
                PassOrDie(caller, GENERALEXCEPT)
            break;
        }
}