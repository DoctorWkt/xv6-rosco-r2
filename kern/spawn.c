// Replace the current program with
// a new program.

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fcntl.h>

#define MAX_RAM		(1024 * 1024)	// 1M of RAM
#define START_ADDR	0x18000

// We use variables that are not on the stack
// to ensure that they are not tromped when
// we move the arguments to the top of stack
static int spawn_i;
static int spawn_cnt;
static int spawn_fd;
static int spawn_argc;
static char *spawn_argv[MAXARGS];
static char *spawn_ptr;
static char *spawn_lowptr;
static char **spawn_cptr;


// Spawn argv[0] with the given arguments.
// Return if this fails.
void sys_spawn(int argc, char *argv[]) {

  // Save the argc off the stack
  spawn_argc= argc;

  // If there are no arguments, or too many, fail.
  if (spawn_argc < 1 || spawn_argc > MAXARGS) return;

  // Check that all the argv's are valid
  for (spawn_i=0; spawn_i < spawn_argc; spawn_i++)
    if (argv[spawn_i]==NULL) return;

  // Try to open the program
  spawn_fd= sys_open(argv[0], O_RDONLY);
  if (spawn_fd== -1) return;

  // Copy each argument on to the stack
  // and save the pointer value
  spawn_ptr= (char *)MAX_RAM;
  for (spawn_i= spawn_argc-1; spawn_i>=0; spawn_i--) {
    // Count the strings's length and the NUL.
    spawn_cnt= strlen(argv[spawn_i])+1;
    spawn_ptr -= spawn_cnt;
    memmove(spawn_ptr, argv[spawn_i], spawn_cnt);
    spawn_argv[spawn_i]= spawn_ptr;
  }

  // Move the pointer down to make room
  // for the argc and the argv pointers.
  // Add on two char pointers: one for
  // a NULL pointer after the argv list,
  // and one for the pointer after argc
  // to point to the argv array.
  // Align the pointer on a word boundary.
  // Save a copy for later.
  spawn_ptr -= sizeof(int) + spawn_argc * sizeof(char *);
  spawn_ptr -= 2 * sizeof(char *);
  spawn_ptr = (char *)((int)spawn_ptr & 0xfffffffe);
  spawn_lowptr= spawn_ptr;

  // Copy argc to the stack
  memmove(spawn_ptr, (void *)&spawn_argc, sizeof(int));
  spawn_ptr += sizeof(int);

  // Put in the argv pointer after argc
  spawn_cptr= (char **)spawn_ptr;
  spawn_ptr += sizeof(char *);
  *spawn_cptr= spawn_ptr;

  // Put the argument pointers on the stack
  for (spawn_i=0; spawn_i < spawn_argc; spawn_i++) {
    memmove(spawn_ptr, &(spawn_argv[spawn_i]), sizeof(char *));
    spawn_ptr += sizeof(char *);
  }

  // Finish with the NULL
  spawn_cptr= (char **)spawn_ptr;
  *spawn_cptr= NULL;

  // Load the program into userspace.
  // Do this after we set up the arguments
  // in case we might tromp on them.
  spawn_ptr= (char *)START_ADDR;
  while (1) {
    spawn_cnt= sys_read(spawn_fd, spawn_ptr, 512);
    if (spawn_cnt<=0) break;
    spawn_ptr+= spawn_cnt;
  }
  sys_close(spawn_fd);

  // Set the stack pointer to point at the argc on the stack
  // and jump to the first instruction in the program
  __asm__ (
	"    move.l %[temp],%%sp\n"
	"    jmp 0x18000\n"
	: [temp] "+d"(spawn_lowptr));
}

// When a program exits, we respawn the shell

static char *argv[]= { "/bin/sh" };

void sys_exit()
{
  // Close all file handles
  for (int i=0; i<NOFILE; i++)
    sys_close(i);

  // Repen stdin, stdout, stderr
  sys_open("/tty", O_RDONLY);
  sys_open("/tty", O_WRONLY);
  sys_open("/tty", O_WRONLY);

  sys_spawn(1, argv);
  panic("sys_exit");
}
