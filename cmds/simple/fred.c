#include <sys/types.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

// errno test


int main() {
  int err;
  char ch;

  err= open("/README", O_RDONLY);
  printf("open README, err %d errno %d\n", err, errno);
  err= open("/FOOBALL", O_RDONLY);
  printf("open README, err %d errno %d\n", err, errno);
  return (0);
}
