/*
 * vfprintf - formatted output without ellipsis
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/vfprintf.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
vfprintf(FILE *stream, const char *format, va_list arg)
{
	return _doprnt (format, arg, stream);
}
