
#ifdef __PANDOS_DEBUGGER_ACTIVE__  /* Defined w make debug */

#include "proj_lib.h"

unsigned int debug_panic_loc = 0;

unsigned int debug_page_index, debug_page_no;

unsigned int debug_entry_hi = 0;

int debugger_asid = -1;

#endif