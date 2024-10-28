TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common

all:	
	(cd ansi; make)
	(cd curses; make)
#	(cd float; make)
	(cd math; make)
	(cd other; make)
#	(cd posix; make)
	(cd sys; make)
	(cd stdio; make)
#	(cd termios; make)
#	$(AR) vr libc.a ansi/*.o other/*.o sys/*.o stdio/*.o termios/*.o
	$(AR) vr libc.a ansi/*.o other/*.o sys/*.o stdio/*.o
	$(RANLIB) libc.a
	$(AR) vr libcurses.a curses/*.o
	$(RANLIB) libcurses.a
#	$(AR) vr libfp.a float/*.o
#	$(RANLIB) libfp.a


clean:
	@rm -rf *.a
	(cd ansi; make clean)
	(cd curses; make clean)
#	(cd float; make clean)
	(cd math; make clean)
	(cd other; make clean)
#	(cd posix; make clean)
	(cd stdio; make clean)
#	(cd termios; make clean)
