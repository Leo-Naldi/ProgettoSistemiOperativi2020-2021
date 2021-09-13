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


int io_dev_mutex[6][DEVPERINT];  /* Semafori per la mutua esclusione sui device di IO */ 
								 /* Macro per le righe in sup_exports.h */


static unsigned int ram_top;

static void make_uproc(unsigned int asid, state_t* out_state, support_t* out_sup)
{
	unsigned int i;
	unsigned int excp_status;

	excp_status = ALLOFF | IEPON | IMON | TEBITON;

	out_state->status = ALLOFF | KUPBITON;
	
	SET_ALL_INTERRUPT_ON(*out_state);
	SET_PLT_ON(*out_state);
	
	out_state->entry_hi = ((unsigned int) asid) << ASIDSHIFT;
	out_state->pc_epc = UPROCSTARTADDR;
	out_state->reg_t9 = UPROCSTARTADDR;
	out_state->reg_sp = USERSTACKTOP;


	out_sup->sup_asid = asid;

	for (i = 0; i < USERPGTBLSIZE - 1; i++)
	{
		(out_sup->sup_privatePgTbl)[i].pte_entryHI = 0x80000000 | (i << VPNSHIFT) | (asid << ASIDSHIFT);
		(out_sup->sup_privatePgTbl)[i].pte_entryLO = DIRTYON;  /* V, G = 0 */
	}
	
	(out_sup->sup_privatePgTbl)[USERPGTBLSIZE - 1].pte_entryHI = UPROC_VIRT_STACK | (asid << ASIDSHIFT);
	(out_sup->sup_privatePgTbl)[USERPGTBLSIZE - 1].pte_entryLO = DIRTYON;  /* V, G = 0 */
	
	(out_sup->sup_exceptContext)[PGFAULTEXCEPT].c_pc = (memaddr) pager; 
	(out_sup->sup_exceptContext)[PGFAULTEXCEPT].c_status = excp_status;
	(out_sup->sup_exceptContext)[PGFAULTEXCEPT].c_stackPtr = ram_top - asid * PAGESIZE * 2 + PAGESIZE;
	
	(out_sup->sup_exceptContext)[GENERALEXCEPT].c_pc = (memaddr) gen_excp_handler;
	(out_sup->sup_exceptContext)[GENERALEXCEPT].c_status = excp_status;
	(out_sup->sup_exceptContext)[GENERALEXCEPT].c_stackPtr = ram_top - asid * PAGESIZE * 2;
}

static void init_uprocs()
{
	static state_t states[UPROCMAX];
	static support_t supports[UPROCMAX];
	unsigned int asid;

	RAMTOP(ram_top);

	for (asid = 1; asid <= UPROCMAX; asid++)
	{
		make_uproc(asid, &(states[asid - 1]), &(supports[asid - 1]));

		SYSCALL(CREATEPROCESS,(int) &(states[asid - 1]),(int) &(supports[asid - 1]), 0);
	}
}

void test()
{
	int i, j;

	for (i = 0; i < 6; i++)
		for (j = 0; j < DEVPERINT; j++)
			io_dev_mutex[i][j] = 1;

	init_pager();

	init_uprocs();

	i = 0;

	SYSCALL(PASSEREN, (int) &(i), 0, 0);   /* Finira' con un panic */
}


