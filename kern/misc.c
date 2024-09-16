// Miscellaneous functions

#include <xv6/types.h>
#include <xv6/defs.h>

static time_t curtime= 1725842955L;

// For now, return an incrementing time
// when we are asked for the time. Later on,
// I will get the heartbeat working.
time_t sys_time(time_t * tloc) {

  curtime++;
cprintf("curtime is %d\n", curtime);
  if (tloc != NULL)
   *tloc= curtime;
  return(curtime);
}
