/*
 * fprintf - write output on a stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fprintf.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
fprintf(FILE *stream, const char *format, ...)
{
	va_list ap;
	int retval;
	
	va_start(ap, format);

	retval = _doprnt (format, ap, stream);

	va_end(ap);

	return retval;
}
