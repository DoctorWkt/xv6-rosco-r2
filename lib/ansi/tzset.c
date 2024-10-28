/*
 * tzset - set timezone information
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/tzset.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

/* This function is present for System V && POSIX */

#include	<time.h>
#include	"loc_time.h"

void
tzset(void)
{
	_tzset();	/* does the job */
}
