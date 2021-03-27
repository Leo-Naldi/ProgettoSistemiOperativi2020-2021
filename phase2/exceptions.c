#include "exceptions.h"

void exceHandler(){
    state_t* caller = SAVED_STATE;
    switch (CAUSE_GET_EXCCODE(getCAUSE())) {
        case EXC_INT:
            interrupt_handler(caller);
            break;
        case EXC_MOD: case EXC_TLBL: case EXC_TLBS:
            PassOrDie(caller, PGFAULTEXCEPT);
            break;
        case EXC_SYS:
            SET_PC(*caller, caller->pc_epc + WORD_SIZE);
            syscall_handler(caller);
            break;
        case EXC_ADEL: case EXC_ADES: case EXC_IBE: case EXC_DBE:
            case EXC_BP: case EXC_RI: case EXC_CPU: case EXC_OV:
                PassOrDie(caller, GENERALEXCEPT);
            break;
        }

}
