#include <unistd.h>
#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
// Miscellaneous functions

// Arbitrary as we don't have pages :-)
int getpagesize(void) {
  return(1024);
}

int getdtablesize(void) {
  return(NOFILE);
}
