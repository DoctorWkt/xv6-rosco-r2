/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/strncpy.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>

char *
strncpy(char *ret, register const char *s2, register size_t n)
{
	register char *s1 = ret;

	if (n>0) {
		while((*s1++ = *s2++) && --n > 0)
			/* EMPTY */ ;
		if ((*--s2 == '\0') && --n > 0) {
			do {
				*s1++ = '\0';
			} while(--n > 0);
		}
	}
	return ret;
}
