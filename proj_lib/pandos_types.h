#ifndef PANDOS_TYPES_H_INCLUDED
#define PANDOS_TYPES_H_INCLUDED

/**************************************************************************** 
 *
 * This header file contains utility types definitions.
 * 
 ****************************************************************************/

#include <umps3/umps/types.h>
#include "pandos_const.h"

typedef signed int cpu_t;
typedef unsigned int memaddr;

typedef struct context_t {
    unsigned int c_stackPtr;
    unsigned int c_status;
    unsigned int c_pc;
} context_t;

typedef struct support_t {
    int       sup_asid;             /* process ID					*/
    state_t   sup_exceptState[2];   /* old state exceptions			*/
    context_t sup_exceptContext[2]; /* new contexts for passing up	*/
} support_t;

/* process table entry type */
typedef struct pcb_t {
	/* process queue fields */
    struct pcb_t   *p_next;							/* ptr to next entry			*/
    struct pcb_t   *p_prev; 						/* ptr to previous entry		*/

	/* process tree fields */
	struct pcb_t	*p_prnt, 						/* ptr to parent				*/
					*p_child,						/* ptr to 1st child				*/
					*p_next_sib,					/* ptr to next sibling 			*/
					*p_prev_sib;					/* ptr to prev. sibling			*/
	
	/* process status information */
	state_t     p_s;              /* processor state        */
	int        *p_semAdd;
	cpu_t       p_time;

	/* add more fields here */
    	support_t *p_supportStruct;

}  pcb_t, *pcb_PTR;

/* Semaphre descriptor type */
typedef struct semd_t {

	struct semd_t *s_next;   /* next element in the ASL */
	int           *s_semAdd; /* pointer to the semaphore */
	pcb_t         *s_procQ;   /* pointer to tail of blocked process queue*/

} semd_t, *semd_PTR;

/* Project defined types */

/* Organizzazione dei semafori associati ai device */
typedef struct device_semaphores_list
{
	int sys_timer;               /* Semaforo del timer di sistema */
	int proc_tymer;              /* Semaforo del Process Local Timer */

	union
	{
		struct{
			int disk_sem[DEVPERINT];     /* Semafori per le istanze dei device Disk */
			int tape_sem[DEVPERINT];     /* Semafori per le istanze dei device Tape */
			int ntw_sem[DEVPERINT];      /* Semafori per le istanze dei device di Network */
			int printer_sem[DEVPERINT];  /* Semafori per le istanze dei device Printer */
			int termr_sem[DEVPERINT];    /* Semafori per le istanze dei Terminali in Lettura */
			int termw_sem[DEVPERINT];    /* Semafori per le istanze dei Terminali in Scrittura */ 
		};
		int sem_mat[6][DEVPERINT];       /* Alias per la memoria precedente per permettere accesso matrix style */
	};
} dev_sem_list_t;

/* End of project defined types */


#endif
