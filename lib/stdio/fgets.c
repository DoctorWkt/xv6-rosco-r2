/*
 * fgets.c - get a string from a file
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fgets.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

char *
fgets(char *s, register int n, register FILE *stream)
{
	register int ch;
	register char *ptr;

	ptr = s;
	while (--n > 0 && (ch = getc(stream)) != EOF) {
		*ptr++ = ch;
		if ( ch == '\n')
			break;
	}
	if (ch == EOF) {
		if (feof(stream)) {
			if (ptr == s) return NULL;
		} else return NULL;
	}
	*ptr = '\0';
	return s;
}
