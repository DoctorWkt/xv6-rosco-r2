#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {
  long tim;

  tim=time(NULL);
  printf("%s\n", ctime(&tim));
  return(0);
}
