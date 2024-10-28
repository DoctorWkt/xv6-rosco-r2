/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/memcmp.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>

int
memcmp(const void *s1, const void *s2, size_t n)
{
	register const char *p1 = s1, *p2 = s2;

	if (n) {
		n++;
		while (--n > 0) {
			if (*p1++ == *p2++) continue;
			return *--p1 - *--p2;
		}
	}
	return 0;
}
