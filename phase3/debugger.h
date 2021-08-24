#ifndef __PAHSE3__DEBUGGER__H__
#define __PAHSE3__DEBUGGER__H__

#ifdef __PANDOS_DEBUGGER_ACTIVE__

extern unsigned int debug_panic_loc;
extern unsigned int debug_page_index;
extern unsigned int debug_page_no;
extern unsigned int debug_entry_hi;

extern int debugger_asid;

#endif

#endif
