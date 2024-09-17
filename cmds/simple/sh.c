#include <string.h>
#include <stdlib.h>
#include <cprintf.h>
#include <fcntl.h>
#include <unistd.h>
#include <xv6/param.h>
#include <dirent.h>
#include <unistd.h>

#define MAXARG 20		// Maximum number of arguments
#define MAXLIN 100		// Maximum line size

int realargc;			// Real argc after parsing the command

// Return true if the name matches the given pattern.
// Only ? and * are recognised. This code from Russ Cox:
// https://research.swtch.com/glob
int match(char *pattern, char *name) {
  int px = 0, nx = 0;
  int nextpx = 0, nextnx = 0;
  int plen = strlen(pattern);
  int nlen = strlen(name);
  char c;

  for (; px < plen || nx < nlen;) {
    if (px < plen) {
      c = pattern[px];

      switch (c) {
      case '?':		// Single character wildcard
	if (nx < nlen) {
	  px++;
	  nx++;
	  continue;
	}
	break;

      case '*':		// Zero or more character wildcard
	// Try to match at nx. If that
	// doesn't work out, restart at
	// nx+1 next.
	nextpx = px;
	nextnx = nx + 1;
	px++;
	continue;

      default:			// Ordinary character
	if (nx < nlen && name[nx] == c) {
	  px++;
	  nx++;
	  continue;
	}
      }
    }

    // Mismatch, maybe restart
    if (0 < nextnx && nextnx <= nlen) {
      px = nextpx;
      nx = nextnx;
      continue;
    }
    return (0);
  }
  // Matched all of pattern to all of name. Success.
  return (1);
}

// Deal with redirections and pipelines in the current argv list
void redirect(int argc, char *argv[]) {
  // char seekbuf[MAXLIN];
  int fd;

  // Assume we won't have any redirection
  realargc = argc;

  // Walk the argument list and process redirections: <, >, >> and 2> only.
  // We go to the 2nd-last argument so we can still process the next one.
  for (int i = 0; i < argc - 1; i++) {

    // Stdout redirected to a file
    if (!strcmp(argv[i], ">")) {

      // Open the file. 
      if ((fd = open(argv[i + 1], O_CREAT | O_TRUNC | O_WRONLY)) == -1) {
	cprintf("Cannot open %s\n", argv[i + 1]);
	exit(1);
      }

      // Close stdout and dup the fd down to stdout, then close the fd
      close(1);
      dup(fd);
      close(fd);

      // Set the arg count to before this token. Skip the next token
      if (realargc == argc)
	realargc = i;
      i++;
    }

    // Stdout redirected to a file, appending
    if (!strcmp(argv[i], ">>")) {

      // Open the file for reading and writing, so we can lseek on it.
      if ((fd = open(argv[i + 1], O_WRONLY | O_APPEND)) == -1) {
	cprintf("Cannot open %s\n", argv[i + 1]);
	exit(1);
      }

      // Close stdout and dup the fd down to stdout, then close the fd
      close(1);
      dup(fd);
      close(fd);

      // Read to the end of the file
      // while ((len=kread(1, seekbuf, MAXLIN))>0) ;

      // Set the arg count to before this token. Skip the next token
      if (realargc == argc)
	realargc = i;
      i++;
    }

    // Stdin redirected from a file
    if (!strcmp(argv[i], "<")) {

      // Open the file.
      if ((fd = open(argv[i + 1], O_RDONLY)) == -1) {
	cprintf("Cannot open %s\n", argv[i + 1]);
	exit(1);
      }

      // Close stdin and dup the fd down to stdin, then close the fd
      close(0);
      dup(fd);
      close(fd);

      // Set the arg count to before this token. Skip the next token
      if (realargc == argc)
	realargc = i;
      i++;
    }

    // Stderr redirected to a file
    if (!strcmp(argv[i], "2>")) {

      // Open the file.
      if ((fd = open(argv[i + 1], O_CREAT | O_TRUNC | O_WRONLY)) == -1) {
	cprintf("Cannot open %s\n", argv[i + 1]);
	exit(1);
      }

      // Close stderr and dup the fd down to stderr, then close the fd
      close(2);
      dup(fd);
      close(fd);

      // Set the arg count to before this token. Skip the next token
      if (realargc == argc)
	realargc = i;
      i++;
    }

    // A pipeline
    if (!strcmp(argv[i], "|")) {

      // Open a temporary file for the output of this command
      if ((fd = open("/.pipedata", O_CREAT | O_TRUNC | O_WRONLY)) == -1) {
	cprintf("Cannot open /.pipedata\n");
	exit(1);
      }

      // Close stdout and dup the fd down to stdout, then close the fd
      close(1);
      dup(fd);
      close(fd);

      // Open a file to store the rest of the pipeline command
      if ((fd = open("/.pipecmd", O_CREAT | O_TRUNC | O_WRONLY)) == -1) {
	cprintf("Cannot open /.pipecmd\n");
	exit(1);
      }

      // Set the arg count to before this token.
      if (realargc == argc)
	realargc = i;

      // Write the rest of the pipeline command to the file
      for (i = i + 1; i < argc; i++) {
	write(fd, argv[i], strlen(argv[i]));
	write(fd, " ", 1);
      }

      // Save the file and return now as there's nothing left to process
      close(fd);
      return;
    }
  }
}

// Parse the given line, generating argv
int parse(char *buf, char *argv[]) {
  int wordc;
  char *wordlist[MAXARG + 1];	// List of words after metachar expansion
  int argc = 0;
  int end;
  DIR *D;
  struct dirent *dent;
  char *cmdp = buf;

  // Split the line up into non-whitespace arguments or operators
  for (wordc = 0; wordc < MAXARG; wordc++) {

    // Get an argument. If none left, break the loop
    if ((wordlist[wordc] = strtok(cmdp, " \t\r\n")) == NULL)
      break;

    // Tell strtok to keep going
    cmdp = NULL;
  }

  // No arguments, just restart the shell
  if (wordc == 0)
    exit(0);

  // Copy the words into the argv list for now
  for (int i = 0; i < wordc && argc < MAXARG; i++) {

    // If the word starts with a single or double quote, trim off the
    // characters at each end and add to the argv list
    if ((wordlist[i][0] == '\'') || (wordlist[i][0] == '"')) {
      end = strlen(wordlist[i]);
      wordlist[i][end - 1] = 0;
      argv[argc++] = &wordlist[i][1];
      continue;
    }

    // See if the word contains a pattern to match on, i.e. a '*' or '?'
    if ((index(wordlist[i], '?') != NULL)
	|| (index(wordlist[i], '*') != NULL)) {

      // For now we can only pattern match in this directory
      if (index(wordlist[i], '/') != NULL) {
	cprintf("Pattern matching only in this directory, sorry\n");
	exit(1);
      }

      // Search the current directory for any matches
      D = opendir(".");
      if (D == NULL) {
	cprintf("Can't opendir .\n");
	exit(1);
      }

      // Process each entry
      while ((dent = readdir(D)) != NULL) {

	// Skip empty directory entries
	if (dent->d_name[0] == '\0')
	  continue;

	// cprintf("About to match %s against %s\n", patternlist[i], dent->d_name);

	// If there's a match, add the filename to the argv list
	if (match(wordlist[i], dent->d_name)) {
	  argv[argc++] = strdup(dent->d_name);
	}
      }
      closedir(D);
      free(D);
      continue;
    }

    // Not a pattern or quoted word, add the word to the argv list
    argv[argc++] = wordlist[i];
  }

  return (argc);
}

char readlinebuf[100];

// For now
char *readline(char *prompt) {
  int i = 0;
  char ch;

  write(1, prompt, strlen(prompt));

  while (1) {
    read(0, &ch, 1);		// write(1, &ch, 1);
    if (ch == '\n' || ch == '\r')
      break;

    if (i >= 98)
      continue;
    readlinebuf[i++] = ch;
  }

  readlinebuf[i] = 0;
  return (readlinebuf);
}

int main() {
  int fd, datafd;
  int argc;
  char *buf = NULL;
  char *argv[MAXARG + 1];	// The argument list
  char binbuf[MAXLIN];		// Used to prepend "/bin/" to commands

  // See if there is a pipe command that we need to process
  fd = open("/.pipecmd", O_RDONLY);
  if (fd != -1) {
    // Yes there is. Check that we have pipe data
    datafd = open("/.pipedata", O_RDONLY);
    if (datafd == -1) {
      cprintf("error: pipe command but no pipe data\n");
      unlink("/.pipecmd");
      exit(1);
    }

    // Read in the command from the command file. NUL terminate it.
    buf = (char *) malloc(MAXLIN);
    if (buf == NULL) {
      cprintf("malloc error\n");
      exit(1);
    }

    int cnt = read(fd, buf, MAXLIN);
    close(fd);
    buf[cnt] = 0;

    // Dup the pipe data to be stdin
    close(0);
    dup(datafd);
    close(datafd);

    // Now unlink both pipe files so they can be reused
    unlink("/.pipecmd");
    unlink("/.pipedata");
  }

  // Use readline() to get the input line if there is no pipeline command
  if (buf == NULL) {
    buf = readline("$ ");
    cprintf("\n");
  }

  // Parse the input line, generating argv
  argc = parse(buf, argv);

  // See if the first argument is cd. If so, do the chdir and
  // then exit() which will respawn the shell!
  if (!strcmp(argv[0], "cd")) {
    if (argc == 2) {
      if (chdir(argv[1]) == -1)
	cprintf("Cannot cd to %s\n", argv[1]);
    }
    exit(0);
  }

  // Try to open the argument.
  if ((fd = open(argv[0], O_RDONLY)) == -1) {
    // No such luck. Try putting "/bin/" on the front
    strcpy(binbuf, "/bin/");
    strcat(binbuf, argv[0]);
    if ((fd = open(binbuf, O_RDONLY)) == -1) {
      cprintf("%s: no such command\n", argv[0]);
      exit(0);
    }
    argv[0] = binbuf;
  }

#if 0
  // See if it's an executable: always start with 0x3406
  kread(fd, &i, 2);
  if (i != 0x3406) {
    cprintf("%s: not an executable\n", argv[0]);
    close(fd);
    exit(0);
  }
#endif
  close(fd);

  // Deal with any redirections
  redirect(argc, argv);

  // No arguments, exit
  if (argc == 0)
    exit(0);

  // Put a NULL at the end of the argv[] array
  argv[realargc] = NULL;

#if 0
  // Debug
  cprintf("sh argc %d\n", realargc);
  for (i = 0; i < realargc; i++)
    cprintf("sh argv[%d] %s 0x%x\n", i, argv[i], argv[i]);
#endif

  // Now spawn the new program
  spawn(realargc, argv);
  exit(0);			// Restart ourselves if the spawn() failed
}
