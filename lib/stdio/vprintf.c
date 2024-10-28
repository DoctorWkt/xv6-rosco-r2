/*
 * vprintf - formatted output without ellipsis to the standard output stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/vprintf.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
vprintf(const char *format, va_list arg)
{
	return _doprnt(format, arg, stdout);
}
