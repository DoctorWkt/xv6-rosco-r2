/*
 * fputc.c - print an unsigned character
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fputc.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
fputc(int c, FILE *stream)
{
	return putc(c, stream);
}
