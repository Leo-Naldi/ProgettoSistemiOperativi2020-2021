/*
 * praticamente devo controllare se eravamo in kernel mode KUp
 * in base a Code.ExceCode richiamo un Handler differente
 */

void exceHandler(){
    state_t* caller = SAVED_STATE;
    switch (CAUSE_GET_EXCCODE(getCAUSE())) {
        case EXC_INT:
            // interrupt handler
            break;
        case EXC_MOD: case EXC_TLBL: case EXC_TLBS:
            //TLB Handler (Pass Up or DIe)
                break;
        case EXC_SYS:
            // Controlla KUP && registro a0 di caller ho un 0..8
            //in caso settare excode to RI e chiamare trap handler (GOTO ???????)
            // Syscall Handler
            break;
        case EXC_ADEL: case EXC_ADES: case EXC_IDE: case EXC_DBE:
            case EXC_BP: case EXC_RI: case EXC_CPU: case EXC_OV:
                // Standard Pass Up or Die
            break;
        default:

            break;
    }

    // SCHEDULER()
}