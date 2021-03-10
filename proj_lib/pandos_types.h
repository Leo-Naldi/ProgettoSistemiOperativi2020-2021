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

/* Struct per rappresentare un pass up vector */
typedef struct PassUpVector
{
	unsigned int* tlb_refill_handler;     /* Indirizzo del gestore dei tbl-refill events */
	unsigned int* tlb_refill_sp;          /* SP per il gestore dei tlb refill events */
	unsigned int* ker_exception_handler;  /* Handler per le eccezioni */
	unsigned int* ker_exception_sp;       /* SP da passare all'handler delle eccezioni */

} pu_vec_t;

#endif
