enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Saved registers for kernel context switches. The layout of the
// context matches the register move code in swtch().
struct context {
  uint d2;	// Offset  0
  uint d3;	// Offset  4
  uint d4;	// Offset  8
  uint d5;	// Offset 12
  uint d6;	// Offset 16
  uint d7;	// Offset 20
  uint a2;	// Offset 24
  uint a3;	// Offset 28
  uint a4;	// Offset 32
  uint a5;	// Offset 36
  uint a6;	// Offset 40
  uint sp;	// Offset 44
  uint pc;	// Offset 48
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
  struct context context;      // Process' saved state
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
