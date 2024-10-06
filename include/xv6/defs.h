struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct rtcdate;
struct spinlock;
struct sleeplock;
struct xvstat;
struct superblock;

// asmcode.s
void cpu_delay(int ms);
void irq5_install();
void send_ch375_cmd(unsigned char cmd);
void send_ch375_data(unsigned char cmd);
unsigned char read_ch375_data(void);
unsigned char get_ch375_status(void);
void setbasereg(int val);
void swtch(int newbasereg, void * newSP);
void cli();
void sti();

// blk.c
void blkinit(void);
void blkrw(struct buf *b);

// bio.c
void            binit(void);
struct buf*     bread(uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

// clock.c
long sys_time(long *tloc);
void sys_stime(long time);
int sys_sleep(unsigned int N);

// cprintf.c
void            cprintf(char *fmt, ...);
void		panic(char *);

// exec.c
void		sys_exec(char *pathname, char *argv[]);
void		sys_exit();

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, char*, int n);
int             filestat(struct file*, struct xvstat*);
int             filewrite(struct file*, char*, int n);

// frames.c
void		finit(void);
void		freeframes(int pid);
int		allocframes(int pid, int nframes);

// fs.c
void		itrunc(struct inode *ip);
void            readsb(struct superblock *sb);
int             dirlink(struct inode*, char*, uint);
struct inode*   dirlookup(struct inode*, char*, uint*);
struct inode*   ialloc(short);
struct inode*   idup(struct inode*);
void            iinit();
void            ilock(struct inode*);
void            iput(struct inode*);
void            iunlock(struct inode*);
void            iunlockput(struct inode*);
void            iupdate(struct inode*);
int             namecmp(const char*, const char*);
struct inode*   namei(char*);
struct inode*   nameiparent(char*, char*);
int             readi(struct inode*, char*, uint, uint);
void            stati(struct inode*, struct xvstat*);
int             writei(struct inode*, char*, uint, uint);
extern struct inode *cwd;

// kalloc.c
int		kbrk(const void *addr);
void *		ksbrk(int increment);

// log.c
void            initlog(void);
void            log_write(struct buf*);
void            begin_op();
void            end_op();

// proc.c
void		pinit(void);
void		userinit(void);
struct proc *	myproc(void);
void		sched(void);
void		sleep(void *chan);
void		wakeup(void *chan);
int		kill(int pid);

// string.c
#ifndef USE_NATIVE_STRINGS
int             memcmp(const void*, const void*, uint);
void*           memmove(void*, const void*, uint);
void*           memset(void*, int, uint);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);
#endif

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(uint, int*);
int             fetchstr(uint, char**);

// sysfile.c
void		set_errno(int err);
int		sys_dup(int fd);
int		sys_read(int fd, char *p, int n);
int		sys_write(int fd, char *p, int n);
int		sys_close(int fd);
int		sys_fstat(int fd, struct xvstat *st);
int		sys_link(char *old, char *new);
int		sys_unlink(char *path);
int		sys_open(char *path, int omode);
int		sys_mkdir(char *path);
int		sys_chdir(char *path);
int		sys_fchdir(int fd);
int		sys_lseek(int fd, int offset, int base);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))
