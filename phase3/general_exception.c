#include "general_exception.h"

void gen_excp_handler()
{
	support_t* cur_proc_sup_ptr;    /* puntatore alla struttura di supporto del current_process */
	unsigned int exception_code;    /* codice dell'eccezione */             

	cur_proc_sup_ptr = (support_t*) SYSCALL(GETSUPPORTPTR, 0, 0, 0);
	exception_code = CAUSE_GET_EXCCODE((cur_proc_sup_ptr->sup_exceptState)[GENERALEXCEPT].cause);

	switch (exception_code)
	{
		case EXC_SYS:
			support_syscall_handler((cur_proc_sup_ptr->sup_exceptState)[GENERALEXCEPT]);
			break;
		
		default:

			SYSCALL(TERMINATE, 0, 0, 0);
			break;
	}
	
	PANIC();  /* Should Not arrive here */
}
