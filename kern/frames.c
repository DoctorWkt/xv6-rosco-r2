// Expansion RAM is divided up into sixteen 64K frames.
// A process is allocated one or more contiguous frames.
// When a process is running, a hardware base register is
// set to map the process' first frame to the address 0x100000.

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>

#define NFRAME	16
#define FREE	0

// The list of frames and which process owns each
int flist[NFRAME];

// Initialise the frame list
void finit() {
  int i;

  for (i=0; i<NFRAME; i++) flist[i]= FREE;
}

// Given a process-id, free all the frames that this
// process is using.
void freeframes(int pid) {
  int i;

  for (i=0; i<NFRAME; i++) {
    if (flist[i]==pid)
      flist[i]= FREE;
  }
}

// Given a process-id and a number of frames required,
// allocate this many frames to the process. Return
// the value of the base register to use, or -1 if
// there are not enough contiguous frames available.
int allocframes(int pid, int nframes) {
  int i;
  int blist[NFRAME];		// Backup of the list
  int maxstart, maxnum;		// Size and start of biggest
				// contiguous run of frames
  int potstart, potnum;		// Potential next best run

  // We could be called from exec() and the frame
  // allocation here could fail. So, we keep a
  // copy of the frame list and restore the list
  // when there is a failure. This will allow
  // exec() to return to the existing process.
  //
  for (i=0; i<NFRAME; i++) blist[i]= flist[i];

  // Free any frames owned by this process
  freeframes(pid);

  // If we only need one frame, just find
  // the first free one and return it
  if (nframes==1) {
    for (i=0; i<NFRAME; i++)
      if (flist[i]== FREE) {
	flist[i]= pid;
	return(i);
      }

    // No frames available, so restore the list and fail
cprintf("Failed to allocate 1 frame to pid %d\n", pid);
    for (i=0; i<NFRAME; i++) flist[i]= blist[i];
    return(-1);
  }

  // Otherwise, find the biggest contiguous run of
  // frames and allocate from there. This helps to
  // prevent fragmentation of the frame list.
  maxstart= potstart= -1;
  maxnum=0;
  for (i=0; i<NFRAME; i++) {
    // Free frame
    if (flist[i]== FREE) {
      // If this is the start of a run, record it
      if (potstart == -1) {
	potstart= i; potnum=1; continue;
      }

      // Otherwise increment the run size
      potnum++; continue;
    } else {
      // If we were recording a potential run,
      // we have hit the end of the run
      if (potstart != -1) {
	// Overwrite the biggest run if this is better
	if (potnum > maxnum) {
	  maxstart= potstart;
	  maxnum= potnum;
	}
	// Mark no more current run
	potstart= -1;
      }
    }
  }

  // If there are enough frames in the
  // maximum run, then allocate from this
  if (nframes >= maxnum) {
    for (i=0; i < nframes; i++) flist[i+maxstart]= pid;
cprintf("Allocating %d frames starting at %d to pid %d\n", maxnum, maxstart, pid);
    return(maxstart);
  }

  // Not enough frames available, so restore the list and fail
cprintf("Failed to allocate %d frames to pid %d\n", nframes, pid);
  for (i=0; i<NFRAME; i++) flist[i]= blist[i];
  return(-1);
}
