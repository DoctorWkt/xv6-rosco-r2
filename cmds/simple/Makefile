TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

CFLAGS+= -g -D_MINIX
CFLAGS+= -Wl,--oformat=elf32-m68k -T ../../misc/userland_program.ld
CFLAGS+= -Wl,-Map=$@.map
CFLAGS+= -nostdinc -nostdlib
CFLAGS+= -L $(GCCLIBDIR) -L $(LIBDIR)
LDLIBS= -lc -lgcc -lcurses

BINS= animals ascii banner basename btoa cal \
      cat cgrep chmem chmod chown cksum cmp col \
      comic comm compress cp cut date dd decomp16 dhrystone dirname \
      diskcheck diskusage du dw echo expand expr factor fgrep file \
      fix fold fortune getlf grep gres \
      head hd hyphenate id ifdef init inodes join kill last less leave ln \
      look lpr ls machine man men mkdir mkfifo mknod mkproto \
      modem mv od paste pr prep \
      printenv proto rev rm rmdir \
      roff sed setdate sh shar size sleep sort split strings stty sum \
      swapfs sync tail tee test touch traverse tr \
      treecmp tset tsort ttt tty umount unexpand uniq unshar update \
      users uud uue vol wc whatsnew whoami who width 

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
