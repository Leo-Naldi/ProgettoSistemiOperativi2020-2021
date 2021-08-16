/************************************************************************************
 *  SUPPORT LEVEL INIT MODULE
 ************************************************************************************
 * 
 * Copntiene le variabili globali e eventuale codice di inizializzazione, la roba 
 * esportabile sara' in sup_exports.
 *
 * Conterra il processo test
 *
 ************************************************************************************/

#include "proj_lib.h"
#include "pager.h"

#define CURRENT_UPROC 1

int io_dev_mutex[6][DEVPERINT];  /* Semafori per la mutua esclusione sui device di IO */ 
								 /* Macro per le righe in sup_exports.h */

static void make_uproc(int asid, state_t* out_state, support_t** out_sup)
{
	static support_t supports[CURRENT_UPROC];
	pcb_t* proc;
	int i;

	*out_sup = &(supports[asid]);


	out_state->status = ALLOFF | KUPBITON;
	
	SET_ALL_INTERRUPT_ON(*out_state);
	SET_PLT_ON(*out_state);
	
	out_state->entry_hi = asid;
	out_state->pc_epc = out_state->reg_t9 = 0x800000B0;
	out_state->reg_sp = 0xC0000000;


	(*out_sup)->sup_asid = asid;

	for (i = 0; i < USERPGTBLSIZE - 1; i++)
	{
		((*out_sup)->sup_privatePgTbl)[i].pte_entryHI = 0x80000000 | (i << VPNSHIFT) | asid;
		((*out_sup)->sup_privatePgTbl)[i].pte_entryLO = DIRTYON;  /* V, G = 0 */
	}
	
	((*out_sup)->sup_privatePgTbl)[USERPGTBLSIZE - 1].pte_entryHI = 0xBFFFF000 | asid;
	((*out_sup)->sup_privatePgTbl)[USERPGTBLSIZE - 1].pte_entryLO = DIRTYON;  /* V, G = 0 */
	
	/* TODO: set gli exc. state in outsup */

	return proc;
}

static void init_uprocs()
{

}

void test()
{
	int i, j;

	for (i = 0; i < 6; i++)
		for (j = 0; j < DEVPERINT; j++)
			io_dev_mutex[i][j] = 1;

	init_pager();

	init_uprocs();

}


