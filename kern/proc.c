#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fcntl.h>
#define PROC_C 1
#include <xv6/proc.h>
#include <errno.h>

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

  // XXX Any more to do here?
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
// the old process' context.
//
void scheduler() {
  struct proc *oldproc, *p;

  // Save a pointer to the current process entry
  oldproc= proc;

  // Loop looking for a runnable process
  while (1) {
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->state == RUNNABLE) {
        proc = p;		// It's the new current process
        proc->state = RUNNING;	// and it is running

        // Switch to its context and then return
        cprintf("About to swtch(%d,0x%p) to pid %d\n",
		proc->basereg, proc->savedSP, proc->pid);
        swtch(proc, oldproc);
	return;
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

// When a program exits, we reexec the shell

static char *argv[]= { "/bin/sh", NULL };

void sys_exit(int exitvalue)
{
  struct proc *p;
  int fd;

  // Save the exit value
  proc->exitstatus= exitvalue;

#ifdef NOTYET
  if (proc == initproc)
    panic("init exiting");
#endif

  // Close all open files.
  for (fd = 0; fd < NOFILE; fd++) {
    if (proc->ofile[fd]) {
      fileclose(proc->ofile[fd]);
      proc->ofile[fd] = 0;
    }
  }

  // Free the program's memory XXX lose it as wait() will do this
  freeframes(proc->pid);

#ifdef NOTYET
  begin_op();
  iput(proc->cwd);
  end_op();
  proc->cwd = 0;

  // Parent might be sleeping in wait().
  wakeup1(proc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == proc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  proc->state = ZOMBIE;
  sched();
  panic("zombie exit");
#endif

  // Reopen stdin, stdout, stderr
  sys_open("/tty", O_RDONLY);
  sys_open("/tty", O_WRONLY);
  sys_open("/tty", O_WRONLY);

  sys_exec(argv[0], argv);
  panic("sys_exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
// Return the child's exit status in the optional pointer argument
int sys_wait(int *statusptr)
{
  struct proc *p;
  int havekids, pid;

  while (1) {
    // Scan through table looking for zombie children.
    havekids = 0;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->parent != proc)
        continue;
      havekids = 1;
      if (p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        freeframes(p->pid);
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        if (statusptr) {
          *statusptr= p->exitstatus | (p->killed ? 0x100 : 0);
        }
        return(pid);
      }
    }

    // No point waiting if we don't have any children.
    if (!havekids || proc->killed) {
      return(-1);
    }

    // Wait for children to exit.  (See wakeup1 call in exit() above.)
    sleep(proc);
  }
}

// Given a pointer to a proc table
// which is proc's new child, copy
// the memory from proc to the child.
// This is ugly code!
// 
// We keep a copy of np and the current
// stack pointer off the stack.

static struct proc *copynp;
static uint copysp;

void copyaddrspace(struct proc *np) {
  char *srctxt, *dsttxt;	// Base of parent and child code+data+bss
  char *srcstk, *dststk;	// Base of parent and child stack
  char *exprambase;		// Base of expansion RAM
  uint txtcnt, stkcnt;		// Size of code+data+bss and stack

  // Get np off the stack
  copynp= np;

  // Save this process' stack pointer
  // as an offset from the process' base
  __asm__ __volatile__(
        "    move.l %%sp,%0\n"
        : "=r" (copysp));

  // Move to the kernel stack so we can
  // change the base register
  __asm__ ("    move.l #0x100000,%sp");

cprintf("Stack pointer is 0x%x\n", copysp - 0x100000);
cprintf("Current brk is 0x%x\n", proc->curbrk - 0x100000);

  // Now that we are on our own stack, calculate
  // how much memory to copy.
  // Expansion RAM starts here.
  exprambase= (char *)0x100000;

  // Use the base regs to work out
  // where each process' memory starts.
  // The brk value is how much to copy.
  srctxt= &exprambase[proc->basereg << 16];
  dsttxt= &exprambase[copynp->basereg << 16];
  txtcnt= proc->curbrk - 0x100000;

  // The stack pointer tells us the base
  // of the stack, but we must add on the
  // base register offsets
  srcstk= &exprambase[(proc->basereg << 16) + copysp - 0x100000];
  dststk= &exprambase[(copynp->basereg << 16) + copysp - 0x100000];

  // Work out where the parent's memory space ends
  // and then subtract the stack pointer to get the
  // amount to copy
  stkcnt= 0x100000 + (copynp->nframes << 16) - copysp;

cprintf("Copying txt: %d bytes from 0x%x to 0x%x\n", txtcnt, srctxt, dsttxt);
cprintf("Copying stk: %d bytes from 0x%x to 0x%x\n", stkcnt, srcstk, dststk);

  // Set the base register to zero so that
  // we can see all the expansion RAM
  setbasereg(0);

  // Copy the parent's text, data and bss.
  // Then copy the parent's stack
  memmove(dsttxt, srctxt, txtcnt);
  memmove(dststk, srcstk, stkcnt);

  // Restore the base register of the parent
  setbasereg(proc->basereg);

  // Restore the stack pointer. We also do
  // the manual work of restoring registers
  // and returning. Why? Because the compiler
  // delays the popping of function arguments
  // until _after_ this _asm_ code runs, so it
  // destroys the correct stack pointer value, sigh.
  __asm__ (
        "    move.l %[temp],%%sp\n"
        : [temp] "+d"(copysp));
  __asm__ ( "    moveml %sp@+,%d2-%d6/%a2-%a3");
  __asm__ ( "    rts");
}

// Create a new process copying proc as the parent.
// Returns -1 if failure, 0 if child, pid if parent.
int sys_fork() {
  struct proc *np;
  void *cursp;
  int i, basereg;

  // Allocate a new process.
  if ((np = allocproc()) == 0) {
    set_errno(EAGAIN); return(-1);
  }

cprintf("Allocated process with pid %d\n", np->pid);

  // Allocate memory to the new process
  basereg= allocframes(np->pid, proc->nframes);
  if (basereg==-1) {
    np->state = UNUSED;
    set_errno(ENOMEM); return(-1);
  }

  // Set up the proc table entry
  np->basereg= basereg;
  np->nframes= proc->nframes;
  np->parent = proc;
  np->chan= (void *)0;
  np->killed= 0;
  safestrcpy(np->name, proc->name, sizeof(proc->name));

cprintf("Allocated %d frames at breg %d to %s\n",
	np->nframes, np->basereg, np->name);

  // Copy over the fds and the cwd
  for (i = 0; i < NOFILE; i++)
    if (proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  // Copy the parent's memory to the child
  // Because we saved the state of the stack
  // at this point, the child will return
  // immediately after this function call.
  // We keep an off-stack copy of np so that
  // the comparison after the copy will work
  copynp= np;
  copyaddrspace(np);

  // If np is the running process, then we
  // are the child. Simply return 0 :-)
  if (copynp == proc)
    return(0);

  // Mark both parent and child as runnable
  proc->state= np->state = RUNNABLE;

  // Use the parent's stack pointer
  // to set up the child's stack pointer.
  // Subtract 72 to make up for the non-existent
  // registers on the stack
  __asm__ __volatile__(
          "    move.l %%sp,%0\n"
          : "=r" (cursp));
  np->savedSP= cursp - 72;

  // Call scheduler()
  cprintf("Entering the scheduler, current pid %d\n", proc->pid);
  scheduler();

  cprintf("Back from the scheduler, current pid %d\n", proc->pid);

  // Return the pid as we are the parent
  return(np->pid);
}

int sys_brk(const void *addr) {
  uint cursp;

  // Get the current stack pointer
  __asm__ __volatile__(
        "    move.l %%sp,%0\n"
        : "=r" (cursp));

  // If the desired address is below bssend
  // or too close to the stack, return -1
  if ((uint)addr < proc->bssend || (uint)addr > (cursp-256)) {
    set_errno(ENOMEM);
    return(-1);
  }

  // Otherwise update the curbrk
  proc->curbrk= (uint)addr; return(0);
}

void *sys_sbrk(int increment) {

  // Keep the old brk value,
  // work out the desired brk value
  uint prevbrk= proc->curbrk;
  uint newbrk= proc->curbrk + increment;

  // See if that works or not
  if (sys_brk((void *)newbrk)== -1) {
    return( (void *) -1 );
  } else
    return((void *)prevbrk);
}
