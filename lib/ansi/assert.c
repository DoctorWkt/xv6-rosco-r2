/*
 * assert.c - diagnostics
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/assert.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<assert.h>
#include	<stdio.h>
#include	<stdlib.h>

void __bad_assertion(const char *mess) {

	fputs(mess, stderr);
	abort();
}
