/*
 * fgetpos.c - get the position in the file
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fgetpos.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
fgetpos(FILE *stream, fpos_t *pos)
{
	*pos = ftell(stream);
	if (*pos == -1) return -1;
	return 0;
}
