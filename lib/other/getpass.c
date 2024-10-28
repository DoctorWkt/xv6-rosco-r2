/*
 * getpass - ask for a password
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/other/RCS/getpass.c,v 1.2 2024/10/26 21:28:42 wkt Exp $ */

#include	<sys/types.h>
#include	<signal.h>
#include	<string.h>
#include	<sgtty.h>
#include	<fcntl.h>

_PROTOTYPE(char *getpass, (_CONST char *prompt ));

#ifdef _ANSI
void (*savesig)(int);
#else
void (*savesig)();
#endif

char *
getpass(prompt)
_CONST char *prompt;
{
	int i = 0;
	struct sgttyb tty, ttysave;
	static char pwdbuf[9];
	int fd;

	if ((fd = open("/dev/tty", O_RDONLY)) < 0) fd = 0;
	savesig = signal(SIGINT, SIG_IGN);
	write(2, prompt, strlen(prompt));
	gtty(fd, &tty);
	ttysave = tty;
	tty.sg_flags &= ~ECHO;
	stty(fd, &tty);
	i = read(fd, pwdbuf, 9);
	while (pwdbuf[i - 1] != '\n')
		read(fd, &pwdbuf[i - 1], 1);
	pwdbuf[i - 1] = '\0';
	stty(fd, &ttysave);
	write(2, "\n", 1);
	if (fd != 0) close(fd);
	signal(SIGINT, savesig);
	return(pwdbuf);
}
