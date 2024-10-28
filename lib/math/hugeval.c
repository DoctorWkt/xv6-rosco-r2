/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Eck
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/math/RCS/hugeval.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */
#include	<math.h>

double
__huge_val(void)
{
	return 1.0e+1000;	/* This will generate a warning */
}
