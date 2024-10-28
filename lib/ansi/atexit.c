/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/atexit.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdlib.h>

#define	NEXITS	32

extern void (*__functab[NEXITS])(void);
extern int __funccnt;

int
atexit(void (*func)(void))
{
	if (__funccnt >= NEXITS)
		return 1;
	__functab[__funccnt++] = func;
	return 0;
}
