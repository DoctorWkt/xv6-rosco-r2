/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/strrchr.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<string.h>

char *
strrchr(register const char *s, int c)
{
	register const char *result = NULL;

	c = (char) c;

	do {
		if (c == *s)
			result = s;
	} while (*s++ != '\0');

	return (char *)result;
}
