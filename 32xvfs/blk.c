// Simulate a block device with an underlying Unix file

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fs.h>
#include <xv6/buf.h>

static int blkfd=-1;		// File descriptor of open file

// Open the underlying file if not yet done
void blkinit(void) {
  if (blkfd == -1) {
    blkfd= open("fs.img", O_RDWR);
    if (blkfd==-1)
      panic("Unable to open fs.img");
  }
}

// Sync buf with disk.
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
void
blkrw(struct buf *b)
{
  blkinit();
  if((b->flags & (B_VALID|B_DIRTY)) == B_VALID)
    panic("blkrw: nothing to do");

  if (lseek(blkfd, b->blockno * BSIZE, SEEK_SET) <0)
    panic("blkrw: lseek failed");

  if (b->flags & B_DIRTY) {
    if (write(blkfd, b->data, BSIZE)<0)
      panic("blkrw: write failed");
    b->flags &= ~B_DIRTY;
  } else {
    if (read(blkfd, b->data, BSIZE)<0)
      panic("blkrw: read failed");
  }
  b->flags |= B_VALID;
}
