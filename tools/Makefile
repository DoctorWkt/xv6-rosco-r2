TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common

all: mkfs chmem

mkfs: mkfs.c
	cc -o mkfs -g -m32 -DBIG_ENDIAN_CPU mkfs.c

chmem: chmem.c
	cc -o chmem -g -m32 -DBIG_ENDIAN_CPU chmem.c

clean:
	rm -f mkfs chmem
