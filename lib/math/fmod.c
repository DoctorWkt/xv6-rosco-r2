/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Eck
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/math/RCS/fmod.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<math.h>
#include	<errno.h>

double
fmod(double x, double y)
{
	long	i;
	double val;
	double frac;

	if (y == 0) {
		errno = EDOM;
		return 0;
	}
	frac = modf( x / y, &val);

	return frac * y;

/*
	val = x / y;
	if (val > LONG_MIN && val < LONG_MAX) {
		i = val;
		return x - i * y;
	}
*/
}
