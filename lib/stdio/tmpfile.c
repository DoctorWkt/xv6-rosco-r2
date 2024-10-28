/*
 * tmpfile.c - create and open a temporary file
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/stdio/RCS/tmpfile.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#if	defined(_POSIX_SOURCE)
#include	<sys/types.h>
#endif
#include	<stdio.h>
#include	<string.h>
#include	"loc_incl.h"

pid_t _getpid(void);

FILE *
tmpfile(void) {
	static char name_buffer[L_tmpnam] = "/tmp/tmp." ;
	static char *name = NULL;
	FILE *file;

	if (!name) {
		name = name_buffer + strlen(name_buffer);
		name = _i_compute(_getpid(), 10, name, 5);
		*name = '\0';
	}

	file = fopen(name_buffer,"wb+");
	if (!file) return (FILE *)NULL;
	(void) remove(name_buffer);
	return file;
}
