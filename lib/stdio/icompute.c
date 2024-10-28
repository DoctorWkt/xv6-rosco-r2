/*
 * icompute.c - compute an integer
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/icompute.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	"loc_incl.h"

/* This routine is used in doprnt.c as well as in tmpfile.c and tmpnam.c. */

char *
_i_compute(unsigned long val, int base, char *s, int nrdigits)
{
	int c;

	c= val % base ;
	val /= base ;
	if (val || nrdigits > 0)
		s = _i_compute(val, base, s, nrdigits - 1);
	*s++ = (c>9 ? c-10+'a' : c+'0');
	return s;
}
