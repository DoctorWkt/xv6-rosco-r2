TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

CFLAGS+= -g
CFLAGS+= -Wl,--oformat=elf32-m68k -T ../../misc/userland_program.ld
CFLAGS+= -Wl,-Map=$@.map
CFLAGS+= -nostdinc -nostdlib
CFLAGS+= -L $(GCCLIBDIR) -L $(LIBDIR)
LDLIBS= -lc -lgcc

BINS= oldcat

# Lose the implicit compilation rule
%: %.c

%.elf: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%: %.elf
	m68k-elf-objcopy -O binary $< $@
	m68k-elf-objdump --disassemble -S $< > $@.dis

all: $(BINS)
	cp $(BINS) ../../build/bin

clean:
	rm -f *.o *.bin *.elf *.dis *.map $(BINS)
