#include "general_exception.h"

#ifdef __PANDOS_DEBUGGER_ACTIVE__

#include "debugger.h"

#endif


void gen_excp_handler()
{
	support_t* cur_proc_sup_ptr;    /* puntatore alla struttura di supporto del current_process */
	state_t* caller;                /* exception state */
	unsigned int exception_code;    /* codice dell'eccezione */             	

	cur_proc_sup_ptr = (support_t*) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
	caller = &((cur_proc_sup_ptr->sup_exceptState)[GENERALEXCEPT]);
	exception_code = CAUSE_GET_EXCCODE(caller->cause);

	switch (exception_code)
	{
		case EXC_SYS:
			support_syscall_handler(caller);
			break;
		
		default:
#ifdef __PANDOS_DEBUGGER_ACTIVE__

			debug_panic_loc = DEBUG_GENERAL_EXCEPTION_ID + 2;
			debug_cause = caller->cause;
			debug_exc_code = exception_code;

			PANIC();  /* Should Not arrive here */
#endif

			SYSCALL(TERMINATE, 0, 0, 0);
			break;
	}
#ifdef __PANDOS_DEBUGGER_ACTIVE__

	debug_panic_loc = DEBUG_GENERAL_EXCEPTION_ID + 1;

	PANIC();  /* Should Not arrive here */
#endif
}
