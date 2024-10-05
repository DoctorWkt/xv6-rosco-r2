#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <endian.h>
#include "../include/a.out.h"

void usage() {
  fprintf(stderr, "Usage: chmem num file, num is in kilobytes\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  long size, cursize;
  int fd, cnt;
  struct aout ahdr;

  // Check we have arguments
  if (argc != 3) usage();

  // Get the new size in bytes
  size= atol(argv[1]);
  if (size <=0 || size > 1024) {
    fprintf(stderr, "%s: either not a number or outside range 0..1024\n");
    usage();
  }
  size *= 1024;

  // Try to open the file
  fd= open(argv[2], O_RDWR);
  if (fd==-1) {
    fprintf(stderr, "Unable to open %s\n", argv[2]); exit(1);
  }

  // Read in the header
  cnt= read(fd, (char *)&ahdr, sizeof(struct aout));
  if (cnt!= sizeof(struct aout) || be16toh(ahdr.a_magic) != AOUT_MAGIC) {
    fprintf(stderr, "%s: not in a.out format\n", argv[2]); exit(1);
  }
  
  // Is the existing program bigger than what is requested?
  cursize= be32toh(ahdr.a_bss_end) - 0x100000;
  if (size < cursize) {
    fprintf(stderr, "%s: needs at least %d bytes, not %d bytes\n",
		argv[2], cursize, size); exit(1);
  }

  // Set the header to have the new size
  ahdr.a_total_size= htobe32(size);

  // Rewrite the header
  lseek(fd, 0, SEEK_SET);
  write(fd, (char *)&ahdr, sizeof(struct aout));
  return(0);
}
