/*
 * puts.c - print a string onto the standard output stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/puts.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
puts(register const char *s)
{
	register FILE *file = stdout;
	register int i = 0;

	while (*s) {
		if (putc(*s++, file) == EOF) return EOF;
		else i++;
	}
	if (putc('\n', file) == EOF) return EOF;
	return i + 1;
}
