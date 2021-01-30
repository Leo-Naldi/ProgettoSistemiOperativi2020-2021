#include "proj_lib/proj_lib.h"

int main()
{
	term_puts("hello, world\n");

	/* Go to sleep and power off the machine if anything wakes us up */
	WAIT();
	*((u32 *) MCTL_POWER) = 0x0FF;
	while (1) ;
}

