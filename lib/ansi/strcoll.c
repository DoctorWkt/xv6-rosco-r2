/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/strcoll.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>
#include	<locale.h>

int
strcoll(register const char *s1, register const char *s2)
{
	while (*s1 == *s2++) {
		if (*s1++ == '\0') {
			return 0;
		}
	}
	return *s1 - *--s2;
}
