// init: The initial user-level program

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>

char *argv[] = { "/bin/sh", NULL };

char *forkfailed= "init: fork failed\n";
char *execshfailed= "init: exec /bin/sh failed\n";

void main() {
  int pid, wpid;

  // Open the console
  open("/tty", O_RDWR);
  dup(0);			// stdout
  dup(0);			// stderr

  while (1) {
    pid = fork();
    if (pid < 0) {
      write(2, forkfailed, strlen(forkfailed));
      exit(1);
    }
    if (pid == 0) {
      execv("/bin/sh", argv);
      write(2, execshfailed, strlen(execshfailed));
      exit(1);
    }
    while ((wpid = wait(NULL)) >= 0 && wpid != pid);	// Clean up zombies
  }
}
