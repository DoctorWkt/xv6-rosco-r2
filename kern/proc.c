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

// Create a new process copying proc as the parent.
// Returns -1 if failure, 0 if child, pid if parent.
int sys_fork() {
  struct proc *np;
  int i, basereg;
  int pid;

  // Allocate a new process.
  if ((np = allocproc()) == 0) {
    set_errno(EAGAIN); return(-1);
  }

  // Allocate memory to the new process
  basereg= allocframes(proc->pid, proc->nframes);
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

  // Copy over the fds and the cwd
  for (i = 0; i < NOFILE; i++)
    if (proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  // Copy the parent's text, data and bss

  // Copy the parent's stack

  // Mark the process as runnable
  np->state = RUNNABLE;

  // Return the pid, or 0 if the child XXX TO DO!!
  pid = np->pid;
  return(pid);
}
