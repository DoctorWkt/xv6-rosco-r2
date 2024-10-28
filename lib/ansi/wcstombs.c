/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/wcstombs.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdlib.h>
#include	<locale.h>
#include	<limits.h>

size_t
wcstombs(register char *s, register const wchar_t *pwcs, size_t n)
{
	register int i = n;

	while (--i >= 0) {
		if (!(*s++ = *pwcs++))
			break;
	}
	return n - i - 1;
}
