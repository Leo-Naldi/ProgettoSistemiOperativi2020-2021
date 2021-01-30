#ifndef PROJ_LIB_H
#define PROJ_LIB_H

#include "umps/types.h"

#include "umps/libumps.h"
#include "umps/arch.h"

#define ST_READY           1
#define ST_BUSY            3
#define ST_TRANSMITTED     5

#define CMD_ACK            1
#define CMD_TRANSMIT       2

#define CHAR_OFFSET        8
#define TERM_STATUS_MASK   0xFF

typedef unsigned int u32;

// Stampa str sul terminale 0
void term_puts(const char *str);

// Stampa il carattere c sul terminale 0
int term_putchar(char c);
u32 tx_status(termreg_t *tp);

static termreg_t *term0_reg = (termreg_t *) DEV_REG_ADDR(IL_TERMINAL, 0);

#endif
