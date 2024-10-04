#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <a.out.h>
#include <stdlib.h>

void usage() {
  fprintf(stderr, "Usage: size file [file]\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  long textsize, datasize, bsssize, fullsize;
  int i, fd, cnt;
  struct aout ahdr;

  // Check we have arguments
  if (argc < 2) usage();

  printf("    text    data     bss     dec     hex   chmem filename\n");

  for (i= 1; i < argc; i++) {
    // Try to open the file
    fd= open(argv[i], O_RDONLY);
    if (fd==-1) {
      printf("Unable to read %s\n", argv[i]);
      continue;
    }

    // Read in the header
    cnt= read(fd, (char *)&ahdr, sizeof(struct aout));
    if (cnt!= sizeof(struct aout) || ahdr.a_magic != AOUT_MAGIC) {
      printf("%s: not in a.out format\n", argv[i]);
      close(fd); continue;
    }

    textsize= ahdr.a_code_end - START_ADDR;
    datasize= ahdr.a_data_end - START_ADDR - textsize;
    bsssize= ahdr.a_bss_end - START_ADDR - textsize - datasize;
    fullsize= ahdr.a_bss_end - START_ADDR;

    printf("%8d%8d%8d%8d%8x%8d %s\n", textsize, datasize, bsssize,
		fullsize, fullsize, ahdr.a_total_size, argv[i]);
    close(fd);
  }

  return(0);
}
