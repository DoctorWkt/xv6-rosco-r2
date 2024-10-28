/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/strpbrk.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>

char *
strpbrk(register const char *string, register const char *brk)
{
	register const char *s1;

	while (*string) {
		for (s1 = brk; *s1 && *s1 != *string; s1++)
			/* EMPTY */ ;
		if (*s1)
			return (char *)string;
		string++;
	}
	return (char *)NULL;
}
