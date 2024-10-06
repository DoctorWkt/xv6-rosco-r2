#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#define PROC_C 1
#include <xv6/proc.h>

struct {
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;	// The init process

int nextpid = 1;

void pinit(void) {
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    p->state = UNUSED;
  proc=NULL;
}

// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc* allocproc(void) {
  struct proc *p;
  char *sp;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  // XXX More to do here
  return(p);
}

// Set up first user process.
void userinit(void) {
  struct proc *p;
  
  p = allocproc();
  proc= initproc = p;

  // XXX More to do here
  p->cwd = namei("/");
  p->state = RUNNABLE;
}

// Find a process that is ready to run and
// context switch to that process. We return
// in the new process' context having saved
// the old process' context
void scheduler(void) {
  void *cursp;
  struct proc *p;

  // Save this process' stack pointer
  __asm__ __volatile__(
        "    move.l %%sp,%0\n"
        : "=r" (cursp));
  proc->savedSP= cursp;

  // Loop looking for a runnable process
  while (1) {
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->state == RUNNABLE) {

      	proc = p;		// It's the new current process
      	proc->state = RUNNING;	// and it is running

      	// Switch to its context and then return
      	swtch(proc->basereg, proc->savedSP);
      }
    }
    cprintf("Didn't find a runnable process in scheduler()\n");
  }
}

// Sleep on chan.
void
sleep(void *chan)
{
  if (proc== NULL)
    panic("sleep");

  // Go to sleep.
  proc->chan = chan;
  proc->state = SLEEPING;
  scheduler();

  // Tidy up.
  proc->chan = 0;
}

// Wake up all processes sleeping on chan.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if (p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  wakeup1(chan);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if (p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if (p->state == SLEEPING)
        p->state = RUNNABLE;
      return 0;
    }
  }
  return -1;
}
