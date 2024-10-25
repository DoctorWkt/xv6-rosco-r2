TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common

all: libc.a libtermcap.a libcurses.a

libc.a:
	(cd gen; make)
	(cd stdio; make)
	(cd stdlib; make)
	(cd string; make)
	(cd locale; make)
	(cd other; make)
	(cd regex; make)
	(cd regexp; make)
	(cd sys; make)
	$(AR) vr libc.a gen/*.o stdio/*.o stdlib/*.o string/*.o \
		locale/*.o other/*.o regex/*.o regexp/*.o sys/*.o
	$(RANLIB) libc.a

libtermcap.a:
	(cd libterm; make)
	$(AR) vr libtermcap.a libterm/*.o
	$(RANLIB) libtermcap.a

libcurses.a:
	make -C libcurses all
	$(AR) vr libcurses.a libcurses/*.o
	$(RANLIB) libcurses.a

clean:
	(cd sys; make clean)
	(cd gen; make clean)
	(cd stdio; make clean)
	(cd stdlib; make clean)
	(cd string; make clean)
	(cd libcurses; make clean)
	(cd libterm; make clean)
	(cd locale; make clean)
	(cd other; make clean)
	(cd regex; make clean)
	(cd regexp; make clean)
	rm -f *.a
