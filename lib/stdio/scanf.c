/*
 * scanf.c - read formatted input from the standard input stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/scanf.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
scanf(const char *format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	retval = _doscan(stdin, format, ap);

	va_end(ap);

	return retval;
}


