/*
 * rewind.c - set the file position indicator of a stream to the start
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/rewind.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	"loc_incl.h"

void
rewind(FILE *stream)
{
	(void) fseek(stream, 0L, SEEK_SET);
	clearerr(stream);
}
