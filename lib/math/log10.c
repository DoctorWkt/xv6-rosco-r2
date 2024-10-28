/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/math/RCS/log10.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<math.h>
#include	<errno.h>
#include	"localmath.h"

double
log10(double x)
{
	if (__IsNan(x)) {
		errno = EDOM;
		return x;
	}
	if (x < 0) {
		errno = EDOM;
		return -HUGE_VAL;
	}
	else if (x == 0) {
		errno = ERANGE;
		return -HUGE_VAL;
	}

	return log(x) / M_LN10;
}
