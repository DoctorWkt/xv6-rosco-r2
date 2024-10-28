/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/raise.c,v 1.2 2024/10/27 00:59:45 wkt Exp $ */

#if	defined(_POSIX_SOURCE)
#include	<sys/types.h>
#endif
#include	<signal.h>

int
raise(int sig)
{
	if (sig < 0 || sig > _NSIG)
		return -1;
	return kill(getpid(), sig);
}
