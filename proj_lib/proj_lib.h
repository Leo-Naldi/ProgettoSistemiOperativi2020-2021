#ifndef PROJ_LIB_H
#define PROJ_LIB_H

#include "umps/types.h"
#include "umps/const.h"
#include "umps/libumps.h"
#include "umps/arch.h"

#include "pandos_const.h"
#include "pandos_types.h"


#define ST_READY           1
#define ST_BUSY            3
#define ST_TRANSMITTED     5

#define CMD_ACK            1
#define CMD_TRANSMIT       2

#define CHAR_OFFSET        8
#define TERM_STATUS_MASK   0xFF


/* Setta a 1 il bit KUp (non c per tenere conto della POP di LDST) */
#define SET_KER_ON(STATE) ((STATE).status &= KUPBITOFF)

/* Setta a 1 il bit IEP e i bit della IM */
#define SET_ALL_INTERRUPT_ON(STATE) ((STATE).status |= IEPON | IMON)

/* Enable the Process Local Timer, should always be called on a newly created process */
#define SET_PLT_ON(STATE) ((STATE).status |= TEBITON)

/* Setta il program counter (e quindi anche il registro t9) a PC_VAL */
#define SET_PC(STATE, PC_VAL) ((STATE).pc_epc = (STATE).reg_t9 = (memaddr)(PC_VAL))

/* Ritorna tt se il bit KUP == 1, ff altrimenti*/
#define IS_USER_MODE(x) ((x) & KUPBITON)

/* True se PTR e' l-indirizzo di uno dei semafori di devices,
 * DSL_OFFSET sara' l'offset dei semafori di sistema (che si assumono
 * essere in un dev_sem_list_t) 
 * i puntatori sono castati a char* per per evitare warning e poter sommare il sizeof
 * correttamente */
#define IS_DEV_SEMADDR(PTR, DSL_OFFSET) (((char*)(PTR) >= (char*)(DSL_OFFSET)) && \
                                        ((char*)(PTR) < (char*)(DSL_OFFSET) + sizeof(dev_sem_list_t)))

#define GET_DEVREG_ADDR(LINE_NO, DEV_NO) (DEVREG_BASE + ((LINE_NO) - 3) * 0x80 +  \
                                            (DEV_NO * 0x10))


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
 * tranne nel caso in cui n e' 0
 */
void* memcpy(void* dest, const void* src, size_t n);

void update_cpu_usage(pcb_t* p, cpu_t* starting_time);

#endif

