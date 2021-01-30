#include "proj_lib.h"


void term_puts(const char *str)
{
	while (*str)
		if (term_putchar(*str++))
			return;
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

u32 tx_status(termreg_t *tp)
{
	return ((tp->transm_status) & TERM_STATUS_MASK);
}
