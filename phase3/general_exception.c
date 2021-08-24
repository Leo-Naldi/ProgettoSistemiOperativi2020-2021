#include "general_exception.h"

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

			SYSCALL(TERMINATE, 0, 0, 0);
			break;
	}
	
	PANIC();  /* Should Not arrive here */
}
