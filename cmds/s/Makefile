TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

CFLAGS+= -g
CFLAGS+= -Wl,--oformat=elf32-m68k -T ../../misc/userland_program.ld
CFLAGS+= -Wl,-Map=$@.map
CFLAGS+= -nostdinc -nostdlib
CFLAGS+= -L $(GCCLIBDIR) -L $(LIBDIR)
LDLIBS= -lc -lgcc

all: s
	../../tools/chmem 256 s
	cp s ../../build/bin/vi

s.elf:	s.o address.o adjust.o Bman.o buffer.o commands.o \
	keyboard.o lib.o operator.o screen.o Sman.o yank.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

s: s.elf
	m68k-elf-objcopy -O binary $< $@
	m68k-elf-objdump --disassemble -S $< > $@.dis

clean:
	rm -f *.o s *.elf *.dis *.map
