/*
 * getw - read a word from a stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/other/RCS/getw.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

_PROTOTYPE(int getw, (FILE *stream ));

int getw(stream)
register FILE *stream;
{
	register int cnt = sizeof(int);
	int w;
	register char *p = (char *) &w;

	while (cnt--) {
		*p++ = getc(stream);
	}
	if (feof(stream) || ferror(stream)) return EOF;
	return w;
}
