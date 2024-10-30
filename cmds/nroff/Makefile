TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

CFLAGS+= -g -D_MINIX
CFLAGS+= -Wl,--oformat=elf32-m68k -T ../../misc/userland_program.ld
CFLAGS+= -Wl,-Map=$@.map
CFLAGS+= -nostdinc -nostdlib
CFLAGS+= -L $(GCCLIBDIR) -L $(LIBDIR)
LDLIBS= -lc -lgcc -lcurses

all: nroff
	../../tools/chmem 256 nroff
	cp nroff ../../build/bin

nroff: nroff.elf
	m68k-elf-objcopy -O binary $< $@
	m68k-elf-objdump --disassemble -S $< > $@.dis

nroff.elf: main.o command.o text.o io.o macros.o strings.o escape.o low.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%: %.elf
	m68k-elf-objcopy -O binary $< $@
	m68k-elf-objdump --disassemble -S $< > $@.dis

clean:
	rm -f *.o *.bin *.elf *.dis *.map nroff
