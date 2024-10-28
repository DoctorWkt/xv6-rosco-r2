/*
 * setbuf.c - control buffering of a stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/setbuf.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	"loc_incl.h"

void
setbuf(register FILE *stream, char *buf)
{
	(void) setvbuf(stream, buf, (buf ? _IOFBF : _IONBF), (size_t) BUFSIZ);
}
