/* 
 * putw - write an word on a stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/other/RCS/putw.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

_PROTOTYPE(int putw, (int w, FILE *stream ));

int
putw(w, stream)
int w;
register FILE *stream;
{
	register int cnt = sizeof(int);
	register char *p = (char *) &w;

	while (cnt--) {
		putc(*p++, stream);
	}
	if (ferror(stream)) return EOF;
	return w;
}
