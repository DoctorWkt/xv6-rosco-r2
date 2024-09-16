#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* This comes from https://github.com/nyuichi/xv6 */

int tcgetattr(int fd, struct termios *termios_p)
{
  return ioctl(fd, TCGETA, (void *)termios_p);
}

int tcsetattr(int fd, __attribute__((unused)) int optional_actions,
				const struct termios *termios_p)
{
  return ioctl(fd, TCSETA, (void *)termios_p);
}

void cfmakeraw(struct termios *termios_p)
{
  // Ignore optional_actions
  termios_p->c_lflag = 0;
}

int tcsendbreak(__attribute__((unused)) int fd, __attribute__((unused)) int len)
{ return(0); }
