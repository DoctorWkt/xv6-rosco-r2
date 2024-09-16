// brk test

void cprintf(char *fmt, ...);

#include <unistd.h>

int main() {
  int i;
  void *ptr, *ptr2;

  for (i=0; i < 256; i++) {
    ptr= sbrk(0x1000);
    ptr2= sbrk(0);
    cprintf("sbrk try %d gives %p, brk %p\n", i, ptr, ptr2);
  }
  while (1) ;
  return(0);
}
