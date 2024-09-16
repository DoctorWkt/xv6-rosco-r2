#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <cprintf.h>

char buf[512];

int main() {
  FILE *in;
  int cnt;

  in= fopen("/roff_manual", "r");
  if (in==NULL) {
    cprintf("fopen failed\n"); exit(1);
  }

  while (1) {
    cnt= fread(buf, 1, 512, in);
    if (cnt<=0) break;
    fwrite(buf, 1, cnt, stdout);
  }

  fclose(in);
  return(0);
}
