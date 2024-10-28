/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/abs.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdlib.h>

int
abs(register int i)
{
	return i >= 0 ? i : -i;
}
