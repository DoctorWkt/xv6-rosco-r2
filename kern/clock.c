#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>

// Time of Day clock for xv6 - Warren Toomey

char tick_cntr=0;	// 100Hz tick counter, updated by interrupt handler
long epoch_time=0;	// Time since Jan 1, 1970

// Return the time to the user process
long sys_time(long *tloc) {
  if (tloc != NULL)
    *tloc= epoch_time;
  return(epoch_time);
}

// Set the clock
void sys_stime(long time) {
  epoch_time= time;
}

void inc_tick(void) {
  tick_cntr++;
  if (tick_cntr==100) {
    tick_cntr=0;
    epoch_time++;
  }
}
