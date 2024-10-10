#include <sys/types.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

// fork test


void main() {
  int pid;
  int status= 45;

  pid= fork();
  switch (pid) {
    case -1: cprintf("fork failed!\n"); break;
    case 0:  cprintf("In the child\n"); exit(3);
    default: cprintf("In the parent, waiting for child pid %d statusptr %p\n",
		pid, &status);
	     pid= wait(&status);
	     cprintf("Back from wait, got pid %d status %d ptr %p\n",
			pid, status, &status);
	     while (1) ;
  }

  exit(0);
}
