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

/* Processor0 Saved state, the BIOSDATAPAGE macro is defined in umps/const.h
 * the state struct is defined in umps/types.h */
#define P0_SSTATE_PTR (state_t*)BIOSDATAPAGE
#define P0_SSTATE *P0_SSTATE_PTR

/* Getter per i registri a0-a3, leggono il valore salvato dal BIOS prima
 * che venisse lanciata l-eccezione. CREDO che chiamando syscall i quattro
 * parametri vengano salvati anche nello stato. Se cosi' non dovesse essere
 * then i have no idea how to get them.*/
#define GET_A0 P0_SSTATE_PTR->reg_a0
#define GET_A1 P0_SSTATE_PTR->reg_a1
#define GET_A2 P0_SSTATE_PTR->reg_a2
#define GET_A3 P0_SSTATE_PTR->reg_a3

/* Setta il registro v0 a x, not sure se e' o no una cosa che dobbiamo fare a mano */
#define SET_V0(X) P0_SSTATE_PTR->reg_v0 = (unsigned int)X

/* Ritorna il valore contenuto in status.ExcCause (aka la causa dell'eccezione) */
#define GET_EXC_CAUSE (getCAUSE() & (unsigned int)0x0000007A) >> 2


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

/*
 * Copia n byte da src a dest, ne' src ne' dest possono essere NULL 
 */
void* memcpy(void* dest, const void* src, size_t n);

#endif

