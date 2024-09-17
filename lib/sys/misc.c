#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int creat(const char *path, mode_t mode)
{
  return( open(path, O_CREAT|O_WRONLY|O_TRUNC));
}
