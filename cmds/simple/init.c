// init: The initial user-level program

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cprintf.h>

char *argv[] = { "/bin/fred", NULL };

void main() {
  int pid, wpid;

  // Open the console
  open("/tty", O_RDWR);
  dup(0);			// stdout
  dup(0);			// stderr

  while (1) {
    pid = fork();
    cprintf("In init, fork() returned %d\n", pid);
    if (pid < 0) {
      cprintf("init: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
    cprintf("init: starting /bin/fred\n");
      execv("/bin/fred", argv);
      cprintf("init: exec /bin/fred failed\n");
      exit(1);
    }
    cprintf("Init parent, about to wait for any child\n");
    while ((wpid = wait(NULL)) >= 0 && wpid != pid);	// Clean up zombies
  }
}
