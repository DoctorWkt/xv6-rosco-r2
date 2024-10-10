enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Saved registers for kernel context switches.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch()
// at the "Switch stacks" comment. Switch doesn't save the PC explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint d2;
  uint d3;
  uint d4;
  uint d5;
  uint d6;
  uint d7;
  uint a2;
  uint a3;
  uint a4;
  uint a5;
  uint a6;
  uint pc;
};

// Per-process state
struct proc {
  int basereg;		       // Base register
  int nframes;		       // Number of frames used
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  uint curbrk;	               // Current brk value
  uint bssend;	               // Original end of the bss
  void *chan;                  // If non-zero, sleeping on chan
  struct context *context;     // Process' saved state
  int killed;                  // If non-zero, have been killed
  int exitstatus;              // Exit value, suitable for wait()
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging) 
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   expandable heap
//   unused space
//   varying-sized stack

#ifdef PROC_C
struct proc *proc;		// Currently running process
#else
extern struct proc *proc;
#endif
