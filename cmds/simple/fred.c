#include <sys/types.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// tc[gs]etattr() test

struct termios T;

int main() {
  int err;
  char ch;

  err= tcgetattr(0, &T);
  printf("tcgetattr err %d\n", err);
  if (err==-1) return(0);
  printf("lflags: 0x%x\n", T.c_lflag);

  T.c_lflag=0;	// No echo
  err= tcsetattr(0, 0, &T);
  printf("tcsetattr err %d\n", err);
  if (err==-1) return(0);

  err= tcgetattr(0, &T);
  printf("tcgetattr err %d\n", err);
  if (err==-1) return(0);
  printf("lflags: 0x%x\n", T.c_lflag);

  printf("Type stuff (no echo, x to stop): "); fflush(stdout);

  while (1) {
    err= read(0, &ch, 1);
    if (err!=1) break;
    if (ch=='x') break;
    ch= ch + 1;
    write(1, &ch, 1);
  }

  T.c_lflag |= ECHO;	// Echo back on
  err= tcsetattr(0, 0, &T);
  printf("tcsetattr err %d\n", err);
  if (err==-1) return(0);

  printf("Type stuff (echo, x to stop): "); fflush(stdout);

  while (1) {
    err= read(0, &ch, 1);
    if (err!=1) break;
    if (ch=='x') break;
    ch= ch + 1;
    write(1, &ch, 1);
  }

  return (0);
}
