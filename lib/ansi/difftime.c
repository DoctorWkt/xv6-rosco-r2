/*
 * difftime - compute the difference between two calendar times
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/difftime.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<time.h>

double
difftime(time_t time1, time_t time0)
{
	/* be careful: time_t may be unsigned */
	if ((time_t)-1 > 0 && time0 > time1) {
		return - (double) (time0 - time1);
	} else {
		return (double)(time1 - time0);
	}
}
