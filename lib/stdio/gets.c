/*
 * gets.c - read a line from a stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/gets.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

char *
gets(char *s)
{
	register FILE *stream = stdin;
	register int ch;
	register char *ptr;

	ptr = s;
	while ((ch = getc(stream)) != EOF && ch != '\n')
		*ptr++ = ch;

	if (ch == EOF) {
		if (feof(stream)) {
			if (ptr == s) return NULL;
		} else return NULL;
	}

	*ptr = '\0';
	return s;
}
