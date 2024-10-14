#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/proc.h>
#include <xv6/fs.h>
#include <xv6/file.h>
#include <errno.h>

#define FREE 0
#define USED 1

struct pipe {
  char data[PIPESIZE];
  char isfree;
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
};

// We have a fixed number of pipes
static struct pipe plist[NPIPE];

// Initialise the pipe list
void pipeinit(void) {
  for (int i=0; i < NPIPE; i++)
    plist[i].isfree = FREE;
}

// Allocate a pipe and return a pointer to it
struct pipe *palloc() {
  for (int i=0; i<NPIPE; i++)
    if (plist[i].isfree == FREE) {
      plist[i].isfree = USED;
      return(&plist[i]);
    }

  return(NULL);
}

// Free a used pipe
void pfree(struct pipe *p) {
  if (p==NULL)
    panic("pfree: NULL pointer");
  if (p->isfree != USED)
    panic("pfree: pipe not in use");
  p->isfree= FREE;
}

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *p;

  p = 0;
  *f0 = *f1 = NULL;
  if((*f0 = filealloc()) == NULL || (*f1 = filealloc()) == NULL)
    goto bad;
  if((p = palloc()) == NULL)
    goto bad;
  p->readopen = 1;
  p->writeopen = 1;
  p->nwrite = 0;
  p->nread = 0;
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = p;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = p;
  return 0;

//PAGEBREAK: 20
 bad:
  if(p)
    pfree(p);
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  set_errno(ENFILE);
  return(-1);
}

void
pipeclose(struct pipe *p, int writable)
{
  if(writable){
    p->writeopen = 0;
    wakeup(&p->nread);
  } else {
    p->readopen = 0;
    wakeup(&p->nwrite);
  }
  if(p->readopen == 0 && p->writeopen == 0){
    pfree(p);
  } 
}

//PAGEBREAK: 40
int
pipewrite(struct pipe *p, char *addr, int n)
{
  int i;

  for(i = 0; i < n; i++){
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
      if(p->readopen == 0 || proc->killed){
        return -1;
      }
      wakeup(&p->nread);
      sleep(&p->nwrite);
    }
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
  }
  wakeup(&p->nread);  //DOC: pipewrite-wakeup1
  return n;
}

int
piperead(struct pipe *p, char *addr, int n)
{
  int i;

  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
    if(proc->killed){
      return EPIPE;
    }
    sleep(&p->nread);
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(p->nread == p->nwrite)
      break;
    addr[i] = p->data[p->nread++ % PIPESIZE];
  }
  wakeup(&p->nwrite);  //DOC: piperead-wakeup
  return i;
}
