TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common
CFLAGS+= -g

OBJS = crt0.o syscalls.o kinit.o cprintf.o misc.o \
	opendir.o isatty.o stat.o getenv.o gettimeofday.o \
	tcgetattr.o access.o getpwent.o

all: $(OBJS)

clean:
	rm -f *.o
