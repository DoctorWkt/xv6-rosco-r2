#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <xv6/types.h>
#include <xv6/param.h>

// Miscellaneous functions

// Arbitrary as we don't have pages :-)
int getpagesize(void) {
  return(1024);
}

int getdtablesize(void) {
  return(NOFILE);
}

int creat(const char *path, Mode_t mode)
{
  return( open(path, O_CREAT|O_WRONLY|O_TRUNC));
}

gid_t getgid(void) {
  return(0);
}

gid_t getegid(void) {
  return(0);
}

uid_t getuid(void) {
  return(0);
}

uid_t geteuid(void) {
  return(0);
}

int setuid(Uid_t uid) {
  return(0);
}

mode_t umask(Mode_t _cmask) {
  return(_cmask);
}

int chown(const char *_path, Uid_t _owner, Gid_t _group) {
  return(0);
}

static char *kmsg= "kill() not implemented\n";

int kill(pid_t _pid, int _sig) {
  write(2, kmsg, strlen(kmsg));
  errno= EPERM;
  return(-1);
}

int mknod(const char *_name, Mode_t _mode, Dev_t _addr) {
  errno= EINVAL;
  return(-1);
}

int mknod4(const char *name, Mode_t mode, Dev_t dev, long size) {
  errno= EINVAL;
  return(-1);
}

int mkfifo(const char *_path, Mode_t _mode) {
  errno= EACCES;
  return(-1);
}

int rmdir(const char *path) {
  return(unlink(path));
}

int mount(char *_spec, char *_name, int _flag) {
  errno= EACCES;
  return(-1);
}

int umount(const char *_name) {
  errno= EINVAL;
  return(-1);
}
