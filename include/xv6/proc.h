enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  int basereg;		       // Base register
  void *savedSP;	       // Saved stack pointer
  void *chan;                  // If non-zero, sleeping on chan
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
