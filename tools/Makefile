TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common

mkfs: mkfs.c
	cc -o mkfs -g -m32 -DBIG_ENDIAN_CPU mkfs.c

clean:
	rm -f mkfs
