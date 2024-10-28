TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common
CFLAGS+= -g

OBJS= crt0.o syscalls.o kinit.o cprintf.o misc.o \
      isatty.o stat.o getenv.o access.o getpwent.o dup2.o \
      exec.o opendir.o chmod.o tcgetattr.o
# gettimeofday.o tcgetattr.o

all: $(OBJS)

clean:
	rm -f *.o
