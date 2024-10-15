#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <cprintf.h>

char *message= "Hello there\n";
char buf[100];

// pipe test

void main() {
  int pipefd[2];
  int result;

  // Get the pipe fds

  result= pipe(pipefd);
  cprintf("Did pipe(), got %d\n", result);
  if (result!=0) exit(1);

  // Writing down the pipe, include the EOS
  result= write(pipefd[1], message, strlen(message)+1);
  cprintf("Did write(), got %d\n", result);
  if (result<1) exit(1);

  // Reading from the file
  result= read(pipefd[0], buf, 100);
  cprintf("Did read(), got %d\n", result);
  if (result<1) exit(1);

  cprintf("%s\n", buf);
  exit(0);
}
