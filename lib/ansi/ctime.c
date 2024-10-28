/*
 * ctime - convers the calendar time to a string
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/ctime.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<time.h>

char *
ctime(const time_t *timer)
{
	return asctime(localtime(timer));
}
