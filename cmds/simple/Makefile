TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

CFLAGS+= -g
CFLAGS+= -Wl,--oformat=elf32-m68k -T ../../misc/userland_program.ld
CFLAGS+= -Wl,-Map=$@.map
CFLAGS+= -nostdinc -nostdlib
CFLAGS+= -L $(GCCLIBDIR) -L $(LIBDIR)
LDLIBS= -lc -lgcc

BINS= banner basename cal cat cksum cmp comm cp crc cut date dd du dirname \
      echo ed expand fgrep fopen fred grep hd head less ln ls mkdir more mv \
      od pwd rev rm rmdir roff sed setdate sh sleep sort split strings tail \
      tar tee touch tr uniq usertests wc foo

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
