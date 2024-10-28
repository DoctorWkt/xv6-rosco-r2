/*
 * fgetc - get an unsigned character and return it as an int
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fgetc.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
fgetc(FILE *stream)
{
	return getc(stream);
}
