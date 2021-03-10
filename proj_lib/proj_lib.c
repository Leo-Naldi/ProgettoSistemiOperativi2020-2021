#include "proj_lib.h"


static termreg_t *term0_reg = (termreg_t *) DEV_REG_ADDR(IL_TERMINAL, 0);

u32 tx_status(termreg_t *tp)
{
	return ((tp->transm_status) & TERM_STATUS_MASK);
}

int term_putchar(char c)
{
	u32 stat;

	stat = tx_status(term0_reg);
	if (stat != ST_READY && stat != ST_TRANSMITTED)
		return -1;

	term0_reg->transm_command = ((c << CHAR_OFFSET) | CMD_TRANSMIT);

	while ((stat = tx_status(term0_reg)) == ST_BUSY)
		;

	term0_reg->transm_command = CMD_ACK;

	if (stat != ST_TRANSMITTED)
		return -1;
	else
		return 0;
}

void term_puts(const char *str)
{
	while (*str)
		if (term_putchar(*str++))
			return;
}


void* memset(void* str, int c, size_t n)
{
	char* s = (char*) str;

	size_t i;

	for (i = 0; i < n; i++)
	{
		s[i] = (unsigned char) c;
	}

	return str;
}

void* memcpy(void* dest, const void* src, size_t n)
{
	char* d = dest;
	const char* s = src;

	while (n--)
		*d++ = *s++;

	return dest;
}


