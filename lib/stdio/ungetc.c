/*
 * ungetc.c - push a character back onto an input stream
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/ungetc.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>
#include	"loc_incl.h"

int
ungetc(int ch, FILE *stream)
{
	unsigned char *p;

	if (ch == EOF  || !io_testflag(stream,_IOREADING))
		return EOF;
	if (stream->_ptr == stream->_buf) {
		if (stream->_count != 0) return EOF;
		stream->_ptr++;
	}
	stream->_count++;
	p = --(stream->_ptr);		/* ??? Bloody vax assembler !!! */
	/* ungetc() in sscanf() shouldn't write in rom */
	if (*p != (unsigned char) ch)
		*p = (unsigned char) ch;
	return ch;
}
