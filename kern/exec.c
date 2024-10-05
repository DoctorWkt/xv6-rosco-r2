// Replace the current program with a new program.

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fcntl.h>
#include <xv6/proc.h>
#include <errno.h>
#include <a.out.h>

// For some reason, this can't be on the stack. Why?
static  char *lowptr;

// Exec argv[0] with the given arguments.
// Return if this fails.
void sys_exec(char *pathname, char *argv[]) {
  int i, len, cnt, fd, argc;
  int nframes, basereg;
  char *oldsbrk;
  char *ptr;
  char **cptr;
  char *kern_argv[MAXARGS];	// Copy in kernel memory
  char *exec_argv[MAXARGS];	// Copy in new program
  struct aout ahdr;		// a.out header

  // Set no errors yet
  set_errno(0);

  // Count the number of arguments
  for (argc=0; argv[argc]!=NULL; argc++) ;

  // If there are no arguments, or too many, fail.
  if (argc < 1 || argc > MAXARGS) {
    set_errno(EFAULT); return;
  }

  // Check that all the argv's are valid
  for (i=0; i < argc; i++)
    if (argv[i]==NULL) {
      set_errno(EFAULT); return;
    }

  // Try to open the program
  fd= sys_open(argv[0], O_RDONLY);
  if (fd== -1) { set_errno(ENOENT); return; }

  // Ensure the file has an a.out header
  cnt= sys_read(fd, (char *)&ahdr, sizeof(struct aout));
  if (cnt!= sizeof(struct aout)) {
    set_errno(EINVAL); return;
  }

  if (ahdr.a_magic != AOUT_MAGIC) {
    set_errno(EINVAL); return;
  }

  // Determine how many frames the new program needs.
  // Round the a_total_size up to the next 64K boundary
  // if necessary. Return if there is not enough memory
  nframes= (ahdr.a_total_size + 0xffff) >> 16;
  basereg= allocframes(proc->pid, nframes);
  if (basereg==-1) {
    set_errno(ENOMEM); return;
  }
  proc->basereg= basereg;

  // We copy the arguments down to kernel memory,
  // then copy them again to the new program's
  // memory. We do this as the new program may
  // have a different base register value than
  // the old program, and it would be a PITA
  // to keep switching between old & new.

  // Count the size of all the argument strings,
  // including the terminating NUL.
  for (cnt=0, i=0; i< argc; i++)
    cnt += strlen(argv[i])+1;

  // Get a pointer to cnt amount of free kernel memory.
  oldsbrk= ptr= (char *)ksbrk(cnt);
  if (oldsbrk== (void *) -1)
    panic("exec");

  // Copy the arguments down to kernel memory
  for (i=0; i< argc; i++) {
    len= strlen(argv[i])+1;
    memmove(ptr, argv[i], len);
    kern_argv[i]= ptr;
    ptr+= len;
  }

  // With the arguments safe, switch over to the
  // new program's memory
  setbasereg(basereg);

  // Set the top of the program's stack.
  // Ensure that it is word aligned
  ahdr.a_total_size &= 0xfffffffe;
  ptr= (char *)START_ADDR + ahdr.a_total_size;
  // ptr= (char *)START_ADDR + 1024 * 1024;
  // cprintf("Set SP to 0x%p\n", ptr);

  // Copy each argument on to the new program's
  // stack and save the pointer value
  for (i= argc-1; i>=0; i--) {
    // Count the strings's length and the NUL.
    cnt= strlen(kern_argv[i])+1;
    ptr -= cnt;
    memmove(ptr, kern_argv[i], cnt);
    exec_argv[i]= ptr;
  }

  // Move the pointer down to make room
  // for the argc and the argv pointers.
  // Add on two char pointers: one for
  // a NULL pointer after the argv list,
  // and one for the pointer after argc
  // to point to the argv array.
  // Align the pointer on a word boundary.
  // Save a copy for later.
  ptr -= sizeof(int) + argc * sizeof(char *);
  ptr -= 2 * sizeof(char *);
  ptr = (char *)((int)ptr & 0xfffffffe);
  lowptr= ptr;

  // Copy argc to the stack
  memmove(ptr, (void *)&argc, sizeof(int));
  ptr += sizeof(int);

  // Put in the argv pointer after argc
  cptr= (char **)ptr;
  ptr += sizeof(char *);
  *cptr= ptr;

  // Put the argument pointers on the stack
  for (i=0; i < argc; i++) {
    memmove(ptr, &(exec_argv[i]), sizeof(char *));
    ptr += sizeof(char *);
  }

  // Finish with the NULL
  cptr= (char **)ptr;
  *cptr= NULL;

  // Return the allocated kernel memory
  kbrk(oldsbrk);

  // Load the program into userspace.
  // Do this after we set up the arguments
  // in case we might tromp on them.
  ptr= (char *)START_ADDR;
  sys_lseek(fd, 0, 0);
  while (1) {
    cnt= sys_read(fd, ptr, 512);
    if (cnt<=0) break;
    ptr+= cnt;
  }
  sys_close(fd);

  // Set the stack pointer to point at the argc on the stack
  // and jump to the first instruction in the program
  __asm__ (
	"    move.l %[temp],%%sp\n"
	"    jmp 0x100000\n"			// XXX Same as START_ADDR
	: [temp] "+d"(lowptr));
}

// When a program exits, we reexec the shell

static char *argv[]= { "/bin/sh", NULL };

void sys_exit()
{
  // Close all file handles
  for (int i=0; i<NOFILE; i++)
    sys_close(i);

  // Free the program's memory
  freeframes(proc->pid);

  // Reopen stdin, stdout, stderr
  sys_open("/tty", O_RDONLY);
  sys_open("/tty", O_WRONLY);
  sys_open("/tty", O_WRONLY);

  sys_exec(argv[0], argv);
  panic("sys_exit");
}
