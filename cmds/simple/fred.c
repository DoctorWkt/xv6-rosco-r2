#include <sys/types.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

// fork test


void main() {
  int pid;

  pid= fork();
  switch (pid) {
    case -1: printf("fork failed!\n"); break;
    case 0:  printf("In the child\n"); break;
    default: printf("In the parent, child pid %d\n", pid);
  }

  exit(0);
}
