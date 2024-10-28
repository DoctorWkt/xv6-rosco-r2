/*
 * remove.c - remove a file
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/remove.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<stdio.h>

int _unlink(const char *path);

int
remove(const char *filename) {
	return _unlink(filename);
}
