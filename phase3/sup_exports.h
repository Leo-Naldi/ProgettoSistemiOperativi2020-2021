#ifndef _SUP_EXPORTS_H_
#define _SUP_EXPORTS_H_

/* Export del livello di supporto */

#include "proj_lib.h"
#include "general_exception.h"
#include "pager.h"
#include "tlbrefill_handler.h"

extern int io_dev_mutex[6][8];  /* Semafori per la mutua esclusione sui device di IO */ 

extern int master_sem;

#define DISK_ROW 0
#define FLASH_ROW 1
#define NTW_ROW 2
#define PRINTER_ROW 3
#define TERMW_ROW 4
#define TERMR_ROW 5

#endif
