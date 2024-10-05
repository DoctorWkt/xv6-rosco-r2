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

void sched(void) { }

// Sleep on chan.
void
sleep(void *chan, struct spinlock *lk)
{
  if(proc== NULL)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Go to sleep.
  proc->chan = chan;
  proc->state = SLEEPING;
  sched();

  // Tidy up.
  proc->chan = 0;
}

// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
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

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      return 0;
    }
  }
  return -1;
}
