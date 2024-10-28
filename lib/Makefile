TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common

all:	
	(cd ansi; make)
	(cd curses; make)
	(cd math; make)
	(cd other; make)
	(cd sys; make)
	(cd stdio; make)
	$(AR) vr libc.a ansi/*.o other/*.o sys/*.o stdio/*.o
	$(RANLIB) libc.a
	$(AR) vr libcurses.a curses/*.o
	$(RANLIB) libcurses.a


clean:
	@rm -rf *.a
	(cd ansi; make clean)
	(cd curses; make clean)
	(cd math; make clean)
	(cd other; make clean)
	(cd stdio; make clean)
	(cd sys; make clean)
