#ifndef __PAHSE3__DEBUGGER__H__
#define __PAHSE3__DEBUGGER__H__

#ifdef __PANDOS_DEBUGGER_ACTIVE__

#define DEBUG_INIT_UPROC_ID        0x00000000
#define DEBUG_TLBREFILL_HANDLER_ID 0x00001000
#define DEBUG_PAGER_ID             0x00002000
#define DEBUG_GENERAL_EXCEPTION_ID 0x00003000
#define DEBUG_SUPPORT_SYSCALLs_ID  0x00004000

#define DEADLOCK 0xFFFFFFFF

extern unsigned int debug_panic_loc;
extern unsigned int debug_page_index;
extern unsigned int debug_page_no;
extern unsigned int debug_entry_hi;
extern unsigned int debug_cause;

extern unsigned int debug_exc_code;

extern int debugger_asid;

extern int debug_flash_status;

extern int debug_tlbrefill_count;
extern int debug_pager_count;

extern unsigned int debug_badvaddr;
extern unsigned int debug_cur_running;

#endif

#endif
