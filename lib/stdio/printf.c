/*
 * printf - write on the standard output stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/printf.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
printf(const char *format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	retval = _doprnt(format, ap, stdout);

	va_end(ap);

	return retval;
}
