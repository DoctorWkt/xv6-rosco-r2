#include <stdint.h>
#include <unistd.h>
#include <errno.h>

// Linker defines
extern uint32_t _data_start, _data_end, _code_end, _bss_start, _bss_end;

// The current brk value starts at the end of BSS.
// Also cache the end of bss.
static void *curbrk= (void *)&_bss_end;
static void *bssend= (void *)&_bss_end;

int brk(const void *addr) {
  void *cursp;

  // Get the current stack pointer
  __asm__ __volatile__(
        "    move.l %%sp,%0\n"
        : "=r" (cursp));

  // If the desired address is below bssend
  // or too close to the stack, return -1
  if (addr < bssend || addr > (cursp-256)) {
    errno= ENOMEM;
    return(-1);
  }

  // Otherwise update the curbrk
  curbrk= (void *)addr; return(0);
}

void *sbrk(int increment) {

  // Keep the old brk value,
  // work out the desired brk value
  void *prevbrk= curbrk;
  void *newbrk= curbrk + increment;

  // See if that works or not
  if (brk(newbrk)== -1) {
    return( (void *) -1 );
  } else
    return(prevbrk);
}
