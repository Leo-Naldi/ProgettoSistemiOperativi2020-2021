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

	/* add more fields here */

}  pcb_t, *pcb_PTR;


/* Semaphre descriptor type */
typedef struct semd_t {

	struct semd_t *s_next;   /* next element in the ASL */
	int           *s_semAdd; /* pointer to the semaphore */
	pcb_t         *s_procQ;   /* pointer to tail of blocked process queue*/

} semd_t, *semd_PTR;

#endif
