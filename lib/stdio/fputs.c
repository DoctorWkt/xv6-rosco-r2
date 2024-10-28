/*
 * fputs - print a string
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fputs.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
fputs(register const char *s, register FILE *stream)
{
	register int i = 0;

	while (*s) 
		if (putc(*s++, stream) == EOF) return EOF;
		else i++;

	return i;
}
