// Replace the current program with a new program.

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fcntl.h>
#include <errno.h>
#include <a.out.h>

#define MAX_RAM		(START_ADDR + 1024 * 1024)	// 1M of RAM

// We use variables that are not on the stack
// to ensure that they are not tromped when
// we move the arguments to the top of stack
static int exec_i;
static int exec_cnt;
static int exec_fd;
static int exec_argc;
static char *exec_argv[MAXARGS];
static char *exec_ptr;
static char *exec_lowptr;
static char **exec_cptr;


// Spawn argv[0] with the given arguments.
// Return if this fails.
void sys_exec(char *pathname, char *argv[]) {
  int argc;
  struct aout ahdr;		// a.out header

  // Set no errors yet
  set_errno(0);

  // Count the number of arguments
  for (argc=0; argv[argc]!=NULL; argc++) ;

  // Save the argc off the stack
  exec_argc= argc;

  // If there are no arguments, or too many, fail.
  if (exec_argc < 1 || exec_argc > MAXARGS) {
    set_errno(EFAULT); return;
  }

  // Check that all the argv's are valid
  for (exec_i=0; exec_i < exec_argc; exec_i++)
    if (argv[exec_i]==NULL) {
      set_errno(EFAULT); return;
    }

  // Try to open the program
  exec_fd= sys_open(argv[0], O_RDONLY);
  if (exec_fd== -1) { set_errno(ENOENT); return; }

  // Ensure the file has an a.out header
  exec_cnt= sys_read(exec_fd, (char *)&ahdr, sizeof(struct aout));
  if (exec_cnt!= sizeof(struct aout)) {
    set_errno(EINVAL); return;
  }

  if (ahdr.a_magic != AOUT_MAGIC) {
    set_errno(EINVAL); return;
  }

  // Copy each argument on to the stack
  // and save the pointer value
  exec_ptr= (char *)MAX_RAM;
  for (exec_i= exec_argc-1; exec_i>=0; exec_i--) {
    // Count the strings's length and the NUL.
    exec_cnt= strlen(argv[exec_i])+1;
    exec_ptr -= exec_cnt;
    memmove(exec_ptr, argv[exec_i], exec_cnt);
    exec_argv[exec_i]= exec_ptr;
  }

  // Move the pointer down to make room
  // for the argc and the argv pointers.
  // Add on two char pointers: one for
  // a NULL pointer after the argv list,
  // and one for the pointer after argc
  // to point to the argv array.
  // Align the pointer on a word boundary.
  // Save a copy for later.
  exec_ptr -= sizeof(int) + exec_argc * sizeof(char *);
  exec_ptr -= 2 * sizeof(char *);
  exec_ptr = (char *)((int)exec_ptr & 0xfffffffe);
  exec_lowptr= exec_ptr;

  // Copy argc to the stack
  memmove(exec_ptr, (void *)&exec_argc, sizeof(int));
  exec_ptr += sizeof(int);

  // Put in the argv pointer after argc
  exec_cptr= (char **)exec_ptr;
  exec_ptr += sizeof(char *);
  *exec_cptr= exec_ptr;

  // Put the argument pointers on the stack
  for (exec_i=0; exec_i < exec_argc; exec_i++) {
    memmove(exec_ptr, &(exec_argv[exec_i]), sizeof(char *));
    exec_ptr += sizeof(char *);
  }

  // Finish with the NULL
  exec_cptr= (char **)exec_ptr;
  *exec_cptr= NULL;

  // Load the program into userspace.
  // Do this after we set up the arguments
  // in case we might tromp on them.
  exec_ptr= (char *)START_ADDR;
  sys_lseek(exec_fd, 0, 0);
  while (1) {
    exec_cnt= sys_read(exec_fd, exec_ptr, 512);
    if (exec_cnt<=0) break;
    exec_ptr+= exec_cnt;
  }
  sys_close(exec_fd);

  // Set the stack pointer to point at the argc on the stack
  // and jump to the first instruction in the program
  __asm__ (
	"    move.l %[temp],%%sp\n"
	"    jmp 0x100000\n"			// XXX Same as START_ADDR
	: [temp] "+d"(exec_lowptr));
}

// When a program exits, we reexec the shell

static char *argv[]= { "/bin/sh", NULL };

void sys_exit()
{
  // Close all file handles
  for (int i=0; i<NOFILE; i++)
    sys_close(i);

  // Reopen stdin, stdout, stderr
  sys_open("/tty", O_RDONLY);
  sys_open("/tty", O_WRONLY);
  sys_open("/tty", O_WRONLY);

  sys_exec(argv[0], argv);
  panic("sys_exit");
}
