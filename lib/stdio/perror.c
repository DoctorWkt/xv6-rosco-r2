/*
 * perror.c - print an error message on the standard error output
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/perror.c,v 1.2 2024/10/27 00:59:45 wkt Exp $ */

#if	defined(_POSIX_SOURCE)
#include	<sys/types.h>
#endif
#include	<stdio.h>
#include	<errno.h>
#include	<stdio.h>
#include	<string.h>
#include	"loc_incl.h"

void
perror(const char *s)
{
	char *p;
	int fd;

	fd = fileno(stderr);
	fflush(stdout);
	fflush(stderr);
	if (s && *s) {
		write(fd, s, strlen(s));
		write(fd, ": ", 2);
	}
	p = strerror(errno);
	write(fd, p, strlen(p));
	write(fd, "\n", 1);
}
