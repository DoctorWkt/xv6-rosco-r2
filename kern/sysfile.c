//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/stat.h>
#include <xv6/fs.h>
#include <xv6/file.h>
#include <xv6/proc.h>
#include <xv6/fcntl.h>
#include <xv6/termios.h>
#include <errno.h>
#include <utime.h>

extern void consputc(char ch);
extern char consgetc(void);

int errno;                      // The kernel location of errno

void set_errno(int err) {       // and the code to set it
  errno= err;
}

#define SBUFSIZE 512
char seekbuf[SBUFSIZE];         // Buffer to do lseek()

// Given a file descriptor
// return both the descriptor and the corresponding struct file.
static int
argfd(int fd, int *pfd, struct file **pf)
{
  struct file *f;

  if(fd < 0 || fd >= NOFILE || (f=proc->ofile[fd]) == 0) {
    set_errno(EBADF); return -1;
  }
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;

  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd] == 0){
      proc->ofile[fd] = f;
      return fd;
    }
  }
  set_errno(EMFILE);
  return -1;
}

int
sys_dup(int fd)
{
  struct file *f;

  set_errno(0);
  if(argfd(fd, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

int
sys_read(int fd, char *p, int n)
{
  struct file *f;

  set_errno(0);
  if(argfd(fd, 0, &f) < 0 || n < 0 || p==0)
    return -1;

  // If the file is the console, return one character from the UART.
  if (f->type == FD_CONSOLE) {
    *p= consgetc(); return(1);
  }

  // Otherwise read from a file
  return fileread(f, p, n);
}

int
sys_write(int fd, char *p, int n)
{
  struct file *f;
  int i;

  set_errno(0);
  if(argfd(fd, 0, &f) < 0 || n < 0 || p==0)
    return -1;

  // If the file is the console, send the characters here.
  if (f->type == FD_CONSOLE) {
    for (i=0; i<n; i++) {
      consputc(p[i]);
    }
    return(n);
  }

  // Otherwise write to the file
  return filewrite(f, p, n);
}

int
sys_close(int fd)
{
  struct file *f;

  set_errno(0);
  if(argfd(fd, 0, &f) < 0)
    return -1;
  proc->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_fstat(int fd, struct xvstat *st)
{
  struct file *f;

  set_errno(0);
  if(argfd(fd, 0, &f) < 0 || st==0)
    return -1;

  // If the file is the console, set this type in the xvstat struct
  if (f->type == FD_CONSOLE) {
    st->type= T_DEV; return(0);
  } else {
    return filestat(f, st);
  }
}

// Create the path new as a link to the same inode as old.
int
sys_link(char *old, char *new)
{
  char name[DIRSIZ];
  struct inode *dp, *ip;

  set_errno(0);
  if(old==0 || new==0) {
    set_errno(ENOENT);
    return -1;
  }

  begin_op();
  if((ip = namei(old)) == 0){
    end_op();
    set_errno(ENOENT);
    return -1;
  }

  ilock(ip);
  if(ip->type == T_DIR){
    iunlockput(ip);
    end_op();
    set_errno(EPERM);
    return -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  if(dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  set_errno(EEXIST);
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  uint off;
  struct xvdirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

int
sys_unlink(char *path)
{
  struct inode *ip, *dp;
  struct xvdirent de;
  char name[DIRSIZ];
  uint off;

  set_errno(0);
  if(path==0) {
    set_errno(ENOENT);
    return -1;
  }

  begin_op();
  if((dp = nameiparent(path, name)) == 0){
    end_op();
    set_errno(EPERM);
    return -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  if((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  if(ip->type == T_DIR){
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  return 0;

bad:
  iunlockput(dp);
  end_op();
  set_errno(EPERM);
  return -1;
}

static struct inode*
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);

  if((ip = dirlookup(dp, name, 0)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(type)) == 0)
    panic("create: ialloc");

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");

  iunlockput(dp);

  return ip;
}

int
sys_open(char *path, int omode)
{
  int fd;
  struct file *f;
  struct inode *ip= NULL;
  int type= FD_INODE;

  set_errno(0);
  if(path==0 || omode <0) {
    set_errno(ENOENT);
    return -1;
  }

  begin_op();

  // If the filename is "/tty", make a console file descriptor
  if (!strncmp(path, "/tty", 4)) {
    type= FD_CONSOLE;

    // This code copied from below, sigh
    if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
      if(f)
        fileclose(f);
      end_op();
      set_errno(EACCES);
      return -1;
    }
  } else {

    if(omode & O_CREATE){
      ip = create(path, T_FILE, 0, 0);
      if(ip == 0){
        end_op();
        set_errno(EEXIST);
        return -1;
      }
    } else {
      if((ip = namei(path)) == 0){
        end_op();
        set_errno(ENOENT);
        return -1;
      }
      ilock(ip);
      if(ip->type == T_DIR && omode != O_RDONLY){
        iunlockput(ip);
        end_op();
	set_errno(EISDIR);
        return -1;
      }
    }

    if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
      if(f)
        fileclose(f);
      iunlockput(ip);
      end_op();
      set_errno(EACCES);
      return -1;
    }

    if((omode & O_TRUNC)) {
      // Not if someone else has this file open. But ref > 1 when O_CREAT
      // if(ip->ref>1) {
      //  iunlockput(ip);
      //  end_op();
      //  return EACCES;
      //}
      itrunc(ip);
    }
    iunlock(ip);
  }

  end_op();

  f->type = type;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  if (omode & O_APPEND)
    f->off= f->ip->size;
  return fd;
}

int
sys_mkdir(char *path)
{
  struct inode *ip;

  begin_op();
  set_errno(0);
  if(path==0 || (ip = create(path, T_DIR, 0, 0)) == 0){
    end_op();
    set_errno(EINVAL);
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

static int ichdir(struct inode *ip)
{
  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    end_op();
    set_errno(ENOTDIR);
    return -1;
  }
  iunlock(ip);
  iput(proc->cwd);
  end_op();
  proc->cwd = ip;
  return 0;
}

int
sys_chdir(char *path)
{
  struct inode *ip;
  
  begin_op();
  set_errno(0);
  if(path==0 || (ip = namei(path)) == 0){
    end_op();
    set_errno(EINVAL);
    return -1;
  }
  return(ichdir(ip));
}

int sys_fchdir(int fd) {
  struct file *f;

  set_errno(0);
  if (argfd(fd, 0, &f) < 0) {
    set_errno(EBADF); return(-1);
  }
  return(ichdir(f->ip));
}

// lseek code derived from https://github.com/ctdk/xv6
int sys_lseek(int fd, int offset, int base) {
  int newoff=0;
  int zerosize;
  struct file *f;

  set_errno(0);
  if (argfd(fd, 0, &f) < 0 || base<0) {
    set_errno(EINVAL); return -1;
  }

  if (base == SEEK_SET)
    newoff = offset;

  if (base == SEEK_CUR)
    newoff = f->off + offset;

  if (base == SEEK_END)
    newoff = f->ip->size + offset;

  if (newoff < 0) {
    set_errno(EINVAL);
    return -1;
  }
  
  // If the new offset is past the file's current size
  if ((unsigned int)newoff > f->ip->size){
    // Work out how much to add to the file
    // and fill a buffer with zeroes
    zerosize = newoff - f->ip->size;
    memset(seekbuf, 0, SBUFSIZE);

    // Write the buffer until we have reached the new size
    while (zerosize > 0){
      filewrite(f, seekbuf, zerosize);
      zerosize -= SBUFSIZE;
    }
  }

  f->off = newoff;
  return newoff;
}

extern char DUART_ECHO_A;

// For now, only console operations
int sys_ioctl (int fd, unsigned long op, void *arg) {
  struct file *f;
  struct termios *t;

  set_errno(0);
  if (argfd(fd, 0, &f) < 0) {
    set_errno(EINVAL); return -1;
  }

  if (f->type != FD_CONSOLE) {
    set_errno(ENOTTY);
    return -1;
  }

  switch(op) {
    case TCGETA:
      t= (struct termios *)arg;
      if (t==NULL) return(-1);
      if (DUART_ECHO_A==1)
      	t->c_lflag = ECHO | ICANON;
      else
      	t->c_lflag = ICANON;
      return(0);

    case TCSETA:
      t= (struct termios *)arg;
      if (t==NULL) return(-1);

      // Enable or disable echoing
      if (t->c_lflag & ECHO)
	DUART_ECHO_A= 1;
      else
	DUART_ECHO_A= 0;
      return(0);
  }
  set_errno(EINVAL);
  return(-1);
}

int sys_utime(const char *path, const struct utimbuf *timbuf) {
  struct inode *ip= NULL;

  set_errno(0);
  if (path==NULL || timbuf==NULL) {
    set_errno(EACCES);
    return(-1);
  }

  begin_op();

  if ((ip = namei((char *)path)) == NULL) {
    end_op();
    set_errno(ENOENT);
    return(-1);
  }

  ilock(ip);
  ip->mtime= timbuf->modtime;
  iupdate(ip);
  iunlock(ip);

  end_op();
  return(0);
}
