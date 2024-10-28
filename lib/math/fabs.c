/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/math/RCS/fabs.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

double
fabs(double x)
{
	return  x < 0 ? -x : x;
}
