#include "proj_lib.h"

typedef struct coso
{
	char c;
	int a;
} Coso;

int main()
{
	int a = 0, b;

	memcpy(&b, &a, 4);

	if (a != b) term_puts("a and b were different");

	Coso c1, c2;

	c1.c = 'a';
	c1.a = 10;

	memcpy(&c2, &c1, sizeof(Coso));

	if ((c1.c != c2.c) || (c1.a != c2.a)) term_puts("Cosi were not equal");


	term_puts("done");

	/* Go to sleep and power off the machine if anything wakes us up */
	WAIT();
	*((u32 *) MCTL_POWER) = 0x0FF;
	while (1) ;
}

