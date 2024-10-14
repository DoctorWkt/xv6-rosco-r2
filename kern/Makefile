TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common
LDFLAGS= --oformat=elf32-m68k  -z max-page-size=16 -z common-page-size=1
LDFLAGS+= -T $(TOP_SRCDIR)/misc/hugerom_rosco_m68k_program.ld
CFLAGS+= -g

#BLKDEVICE = sdblk.o
BLKDEVICE = usbblk.o

INIT_O = init.o

OBJS =  $(BLKDEVICE) bio.o clock.o cprintf.o file.o fs.o kinit.o \
	log.o main.o exec.o string.o sysfile.o asmcode.o \
	pipe.o proc.o frames.o kalloc.o sdcard_asm.o

all: xv6.bin

xv6.bin: $(INIT_O) $(OBJS)
	$(LD) $(LDFLAGS) -o xv6.elf -Map=xv6.map \
				$(INIT_O) $(OBJS) -L $(GCCLIBDIR) -lgcc
	m68k-elf-objcopy -O binary xv6.elf xv6.bin
	m68k-elf-objdump --disassemble -S xv6.elf > xv6.dis

clean:
	rm -f *.o
	rm -f xv6.bin xv6.elf xv6.dis xv6.map
