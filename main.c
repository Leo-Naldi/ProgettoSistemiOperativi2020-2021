#include "phase1/ProcQ.h"

int main()
{
	say_hello();

	/* Go to sleep and power off the machine if anything wakes us up */
	WAIT();
	*((u32 *) MCTL_POWER) = 0x0FF;
	while (1) ;
}

