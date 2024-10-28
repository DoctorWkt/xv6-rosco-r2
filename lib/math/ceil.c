/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/math/RCS/ceil.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<math.h>

double
ceil(double x)
{
	double val;

	return modf(x, &val) > 0 ? val + 1.0 : val ;
	/*	this also works if modf always returns a positive
		fractional part
	*/
}
