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

// Initialise the proc table
void pinit(void) {
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    p->state = UNUSED;
  proc=NULL;
}

// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return NULL.
static struct proc* allocproc(void) {
  struct proc *p;
  char *sp;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  return(NULL);

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  return(p);
}

// Set up the first user process.
void userinit(void) {
  struct proc *p;
  
  p = allocproc();
  proc= initproc = p;

  // XXX More to do here. We need to take
  // some of the code currently in main()
  // and put it here.
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
#if 0
        cprintf("About to swtch to pid %d basereg %d\n",
		proc->pid, proc->basereg);
#endif
        swtch(&(oldproc->context), &(proc->context), proc->basereg);
	return;
      }
    }
    cprintf("Didn't find a runnable process in scheduler()\n");
  }
}

// A fork()'s child returns via this function.
// Restore the registers as if we were returning
// from fork()
int forkret() {
  __asm__ ( "    moveml %sp@+,%a2-%a5");
  return(0);
}

// Sleep on chan.
void sleep(void *chan) {
  if (chan== NULL || proc== NULL)
    panic("sleep");

  // Go to sleep
  proc->chan = chan;
  proc->state = SLEEPING;
  scheduler();

  // Tidy up after we wake up
  proc->chan = NULL;
  return;
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

// Stop the current process, release its
// resources and get the scheduler to start
// another process.
void sys_exit(int exitvalue)
{
  struct proc *p;
  int fd;

  // Save the exit value
  proc->exitstatus= exitvalue;

  if (proc == initproc)
    panic("init exiting");

  // Close all open files.
  for (fd = 0; fd < NOFILE; fd++) {
    if (proc->ofile[fd]) {
      fileclose(proc->ofile[fd]);
      proc->ofile[fd] = 0;
    }
  }

  // Free the process' memory
  freeframes(proc->pid);

#ifdef NOTYET
  // XXX Not sure why this causes a panic yet
  begin_op();
  iput(proc->cwd);
  end_op();
#endif
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
  scheduler();
  panic("zombie exit");
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
        // Found one. clear the proc table entry
        pid = p->pid;
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        if (statusptr) {
          *statusptr= p->exitstatus | (p->killed ? 0x100 : 0);
        }
        p->killed = 0;
        return(pid);
      }
    }

    // No point waiting if we don't have any children.
    if (!havekids) {
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

  // Save the stack pointer so
  // we can restore it later
  copysp= getsp();

  // XXX
  // XXX Find a way to make all the 0x100000 literals into symbols
  // XXX

  // Move to the kernel stack so we can
  // change the base register
  __asm__ ("    move.l #0x100000,%sp");

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

#if 0
   // If you turn this on, you need to change d5 to d6 in the
   // assembly at the end of this function
   cprintf("Copying txt: %d bytes from 0x%x to 0x%x\n", txtcnt, srctxt, dsttxt);
   cprintf("Copying stk: %d bytes from 0x%x to 0x%x\n", stkcnt, srcstk, dststk);
#endif

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
  __asm__ ( "    moveml %sp@+,%d2-%d5/%a2-%a3");
  __asm__ ( "    rts");
}

// Create a new process copying proc as the parent.
// Returns -1 if failure, 0 if child, pid if parent.
int sys_fork() {
  struct proc *np;
  void *cursp;
  int i, basereg;

  // Get a copy of our current stack pointer
  // before the compiler leaves a whole heap
  // of unpopped arguments on the stack.
  // Make room for the forkret() return address.
  copysp= getsp()-4;

  // Allocate a new process.
  if ((np = allocproc()) == NULL) {
    set_errno(EAGAIN); return(-1);
  }

#if 0
cprintf("Allocated process with pid %d, proc ptr 0x%p\n", np->pid, np);
#endif

  // Allocate memory to the new process
  basereg= allocframes(np->pid, proc->nframes);
  if (basereg==-1) {
    np->state = UNUSED;
    set_errno(ENOMEM); return(-1);
  }

  // Set up the proc table entry
  np->state = RUNNABLE;
  np->basereg= basereg;
  np->nframes= proc->nframes;
  np->parent = proc;
  np->chan= NULL;
  np->killed= 0;
  safestrcpy(np->name, proc->name, sizeof(proc->name));

  // Set the child to return to forkret() with
  // the SP we had at the start of sys_fork()
  // and save a copy of the registers
  np->context.pc= (uint)forkret;
  np->context.sp= copysp;
  saveregs(&(np->context));

  // Copy over the fds and the cwd
  for (i = 0; i < NOFILE; i++)
    if (proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  // Copy the parent's memory to the child
  copyaddrspace(np);

  // Return the pid of the child
  return(np->pid);
}

int sys_brk(const void *addr) {
  uint cursp;

  // Get the current stack pointer
  cursp= getsp();

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

int sys_getpid() {
  return(proc->pid);
}
