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

typedef unsigned int size_t;

/* Stampa str sul terminale 0 */
void term_puts(const char *str);

int term_putchar(char c);

u32 tx_status(termreg_t *tp);

/*
 * Scrive negli n byte a partire da str il valore c (interpretato come unsigned char).
 * Undefined se str e' NULL.
 * */
void* memset(void* str, int c, size_t n);


#endif

