#include <xv6/param.h>
#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/stat.h>
#include <xv6/fs.h>
#include <xv6/fcntl.h>

// Variables visible to other functions

// Location where the interrupt handler
// stores the CH375 status byte
char CH375_STATUS= 0;

// Should we echo characters on DUART A?
// 1 means yes, 0 means no.
char DUART_ECHO_A= 1;

// Our variables
static char *argv[]= { "/bin/sh" };

void kmain()
{
  cprintf("Welcome to xv6\n");
  sys_init();

  // Open stdin, stdout, stderr
  sys_open("/tty", O_RDONLY);
  sys_open("/tty", O_WRONLY);
  sys_open("/tty", O_WRONLY);

  // Start the first program
  sys_spawn(1, argv);
  while (1) ;
}
