/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/strcmp.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>

int
strcmp(register const char *s1, register const char *s2)
{
	while (*s1 == *s2++) {
		if (*s1++ == '\0') {
			return 0;
		}
	}
	if (*s1 == '\0') return -1;
	if (*--s2 == '\0') return 1;
	return *s1 - *s2;
}
