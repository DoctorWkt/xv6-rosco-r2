#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MANDIR "/usr/man"
#define NROFFPROG "/bin/nroff"
#define LESSPROG "/bin/less"

char *lessargv[3]= { LESSPROG, NULL, NULL };

// Run less to display the named file
void displayman(char *filename) {
  lessargv[1]= filename;

// fprintf(stderr,"About to %s %s\n", lessargv[0], lessargv[1]);
  execv(LESSPROG, lessargv);
  fprintf(stderr, "man: %s %s failed\n", LESSPROG, filename);
  exit(1);
}

char *nroffargv[4]= { NROFFPROG, "-man", NULL, NULL };

// Run nroff to convert the man page in filename
// int its pre-formatted form in filename2
void formatman(char *filename, char *filename2) {
  int fd, pid;
  nroffargv[2]= filename;

  switch(pid=fork()) {
    case -1:
	fprintf(stderr, "man: fork failed\n"); exit(1);
    case 0:
	// Open the second file 
	if ((fd = open(filename2, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1) {
	  fprintf(stderr, "man: could not write %s\n", filename2); exit(1);
	}

	// Make this file the standard output
	dup2(fd, 1); close(fd);
// fprintf(stderr,"About to %s %s %s\n", nroffargv[0], nroffargv[1], nroffargv[2]);
  	execv(NROFFPROG, nroffargv);
  	fprintf(stderr, "man: %s %s failed\n", NROFFPROG, filename);
	exit(1);
    default:
	waitpid(pid, NULL, 0);
  }
}

#define BUFSIZE 256
char buf[BUFSIZE];
char buf2[BUFSIZE];

// Find and display a pre-formatted manual.
// Return 1 if OK, 0 otherwise
int findcatman(int section, char *name) {
  int fd;
  
  // Get the possible filename in the given pre-formatted section
  sprintf(buf, "%s/cat%d/%s.%d", MANDIR, section, name, section);
// fprintf(stderr,"Trying to find %s\n", buf);

  // Try to open the filename
  if ((fd = open(buf, O_RDONLY)) == -1)
    return(0);

  // The file exists, display it
// fprintf(stderr, "%s exists\n", buf);
  close(fd); displayman(buf); return(1);
}

// Find and format an unformatted manual.
// Return 1 if OK, 0 otherwise
int findmanman(int section, char *name) {
  int fd; 

  // Get the possible filename in the given unformatted section.
  // Get the possible filename in the given pre-formatted section.
  sprintf(buf, "%s/man%d/%s.%d", MANDIR, section, name, section);
  sprintf(buf2, "%s/cat%d/%s.%d", MANDIR, section, name, section);

  // Try to open the unformatted manual
// fprintf(stderr, "Trying to find %s\n", buf);
  if ((fd = open(buf, O_RDONLY)) == -1)
    return(0);
  
  // It does exist, so format it
// fprintf(stderr, "%s exists, making %s\n", buf, buf2);
  close(fd);
  formatman(buf, buf2);
  return(1);
}

// Try to either display a pre-formatted manual
// or format then display a pre-formatted manual.
// Return 1 if OK, 0 otherwise
int tryman(int section, char *name) {

  // If we can display a pre-formatted one, return
  if (findcatman(section, name)) return(1);

  // No. See if we can format then display it
  if (findmanman(section, name))
    return(findcatman(section, name));

  // Both failed, return zero
  return(0);
}

int main(int argc, char *argv[]) {
  int section;
  char *name= argv[1];

  // If we have two arguments and the first
  // is numeric, try that section
  if (argc==3) {
    name= argv[2];
    if (isdigit(argv[1][0])) {
      section= argv[1][0] - '0';
      if (tryman(section, name)) exit(0);
      fprintf(stderr, "No manual entry for %s\n", name); exit(1);
    } else {
      fprintf(stderr, "man: %s is not a numeric section\n", argv[1]); exit(1);
    }
  }

  // No specific section, try all of them
  for (section=0; section<=8; section++) {
    if (tryman(section, name)) exit(0);
  }

  fprintf(stderr, "No manual entry for %s\n", name); exit(1);
}
