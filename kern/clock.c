#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>

// Time of Day clock for xv6 - Warren Toomey

char tick_cntr=0;		// 100Hz tick counter, updated by interrupt handler
volatile long epoch_time=0;	// Time since Jan 1, 1970

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

// Sleep for N seconds
int sys_sleep(unsigned int N) {
  // Get the future time
  long newtime= epoch_time + (long)N;

  // Loop until the current time
  // matches the future time
  while (epoch_time < newtime)
    ;
  return(0);
}
