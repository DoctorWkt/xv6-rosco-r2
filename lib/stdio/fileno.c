/*
 * fileno .c - map a stream to a file descriptor
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/fileno.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int
(fileno)(FILE *stream)
{
	return stream->_fd;
}
