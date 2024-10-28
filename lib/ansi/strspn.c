/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/strspn.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>

size_t
strspn(const char *string, const char *in)
{
	register const char *s1, *s2;

	for (s1 = string; *s1; s1++) {
		for (s2 = in; *s2 && *s2 != *s1; s2++)
			/* EMPTY */ ;
		if (*s2 == '\0')
			break;
	}
	return s1 - string;
}
