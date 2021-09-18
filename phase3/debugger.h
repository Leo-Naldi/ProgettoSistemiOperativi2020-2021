#ifndef __PAHSE3__DEBUGGER__H__
#define __PAHSE3__DEBUGGER__H__

#ifdef __PANDOS_DEBUGGER_ACTIVE__

/* Id per debug_panic_loc */
#define DEBUG_INIT_UPROC_ID        0x00000000
#define DEBUG_TLBREFILL_HANDLER_ID 0x00001000
#define DEBUG_PAGER_ID             0x00002000
#define DEBUG_GENERAL_EXCEPTION_ID 0x00003000
#define DEBUG_SUPPORT_SYSCALLs_ID  0x00004000

#define DEADLOCK 0xFFFFFFFF

extern unsigned int debug_panic_loc;  /* Per identificare dove e' avvenuta la panic, il file e'inicato dagli ID */
extern unsigned int debug_page_index; /* Indice della pagina nella tavola pte */
extern unsigned int debug_page_no;    /* VPN errato della pagina al momento della panic */
extern unsigned int debug_entry_hi;   /* entry hi dell'eccezione al momento della panic */
extern unsigned int debug_cause;      /* registro cause dell'eccezione al momento della panic */

extern unsigned int debug_exc_code;

extern int debugger_asid;

extern int debug_flash_status;       /* Stato di errore del flash */

extern int debug_tlbrefill_count;    /* Numero di volte in cui e' stato chiamato il tlbrefill_handler */
extern int debug_pager_count;        /* Numero di volte in cui e' stato chiamato il pager */

extern unsigned int debug_badvaddr;  /* Indirizzo che ha causato la panic nel tlbrefill o nel pager */

#endif

#endif
