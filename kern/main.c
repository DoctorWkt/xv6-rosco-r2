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
// Should we convert CR to NL on DUART A?
// 1 means yes, 0 means no.
char DUART_ECHO_A= 1;
char DUART_CRNL_A= 1;

// Our variables
static char *argv[]= { "/bin/init", NULL };

void kmain()
{
  cprintf("Welcome to xv6\n");

  // Initialise several data structures
  blkinit();       // Underlying block device
  binit();         // Buffer cache
  fileinit();      // File table
  iinit();	   // Read the superblock
  initlog();	   // Block logging
  pinit();	   // Process table
  finit();	   // Free frame list
  pipeinit();	   // Pipe list

  // Eventually this will initialise the first program.
  // For now, it sets up the working directory.
  userinit();

  // Start the first program
  sys_exec(argv[0], argv);
  while (1) ;
}
