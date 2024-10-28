/*
 * fsetpos.c - set the position in the file
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fsetpos.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
fsetpos(FILE *stream, fpos_t *pos)
{
	return fseek(stream, *pos, SEEK_SET);
}
