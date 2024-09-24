# xv6-rosco-r2 - xv6 on the Rosco m68k Classic v2

This project aims to port the [xv6](https://github.com/mit-pdos/xv6-public) kernel
to the [Rosco m68k Classic v2](https://github.com/rosco-m68k/rosco_m68k) SBC. The
goal is to have a Unix-like kernel with a BSD userland on top of it, much like my
previous [xv6-freebsd](https://github.com/DoctorWkt/xv6-freebsd) project.

This time, I'm going to do things in a few stages:

 - Stage 1: Bring up a system with just one program running at a time. I've
   done this before with the [Nine-E](https://github.com/DoctorWkt/Nine-E/) project.
 - Stage 2: Move to multiple processes but with no process pre-emption. I've done
   this before with the [MMU09](https://github.com/DoctorWkt/MMU09) project.
 - Stage 3: Enable process pre-emption and then slowly add in all the libraries
   and userland from the [xv6-freebsd](https://github.com/DoctorWkt/xv6-freebsd) project.

Please note: this is a work in progress. It's definitely not a finished and polished
system at the moment. You will have to figure some things out yourself.

## Hardware Options

As at mid-Sept '24, I'm finding the bit-bang SD card very slow. I also really don't
want to have to put in a relocating loader into the kernel. So I plan on designing
an expansion board with 1M of RAM and a
[CH375](https://www.electrodragon.com/product/ch375-module-reading-and-writing-u-diskusb-communicate/)
USB interface. There will be a
[base register](https://en.wikipedia.org/wiki/Base_and_bounds) in front of the RAM, so that I can have multiple processes, but each one will think that they start at the same base address.

## Running the System

You will find a Zip file at the top-level, `sd-img.zip`. Unzip this to extract
the `sdcard.img` file: this is an SD card image. Write this, block for block, onto
an SD card, e.g.

```
$ cat sdcard.img > /dev/sdc              or
$ dd if=sdcard.img of=/dev/sdc bs=1M
```

(Can someone tell me how to do this under MacOS and Windows, thanks!)

Now insert the SD card into your Rosco m68k Classic v2 SBC and boot the system.
You should see something like:

```
                                 ___ ___ _   
 ___ ___ ___ ___ ___       _____|  _| . | |_ 
|  _| . |_ -|  _| . |     |     | . | . | '_|
|_| |___|___|___|___|_____|_|_|_|___|__|_,_|
                    |_____|  Classic 2.50.DEV

MC68020 CPU @ 10MHz with 1048576 bytes RAM
Initializing hard drives... No IDE interface found
Searching for boot media...
  Partition 1: Loading "/ROSCODE1.BIN"...
Loaded 12080 bytes in ~1 sec.

Welcome to xv6
About to initialise the SD card
SD card xv6 partition at 0x9000
xv6 superblock: size 1000 nblocks 961 ninodes 200
  nlog 10 logstart 2 inodestart 12 bmap start 38
$
```

You can then do stuff like:

```
$ ls -l
-rwxrwxrwx     1 root root     46 Thu Jan  1 00:00:00 README
drwxrwxrwx     1 root root    176 Thu Jan  1 00:00:00 bin
drwxrwxrwx     1 root root     64 Thu Jan  1 00:00:00 etc
-rwxrwxrwx     1 root root   6071 Thu Jan  1 00:00:00 roff_manual
$ ls -l bin
-rwxrwxrwx     1 root root  46096 Thu Jan  1 00:00:00 cal
-rwxrwxrwx     1 root root  28524 Thu Jan  1 00:00:00 cat
-rwxrwxrwx     1 root root  17024 Thu Jan  1 00:00:00 echo
-rwxrwxrwx     1 root root  17604 Thu Jan  1 00:00:00 fopen
-rwxrwxrwx     1 root root  17024 Thu Jan  1 00:00:00 fred
-rwxrwxrwx     1 root root  38952 Thu Jan  1 00:00:00 ls
-rwxrwxrwx     1 root root  17024 Thu Jan  1 00:00:00 rm
-rwxrwxrwx     1 root root  17872 Thu Jan  1 00:00:00 sh
-rwxrwxrwx     1 root root  27968 Thu Jan  1 00:00:00 usertests
$
```

## Building the System

I need to put more docs here, but for now ...

You will need the [rosco toolchain](https://rosco-m68k.com/docs/toolchain-installation)
installed on your system as well as `mtools` and `mkfs.vfat`. There are probably a few
more things needed - let me know what I've forgotten.

At the top level, do a `make` to build the kernel, the tools, the libraries and
the commands. This will build the `fs.img` xv6 filesystem image.

Yes, there are a heap of warnings. Over time, I will try to fix them.

Then, do a `make sdcard.img`. This creates the SD card image. The image has two
partitions: the FAT partition has a bootable copy of the `xv6` kernel, and the
second partition holds the `xv6` filesystem.

## Status - 24 Sept 2024

I've imported a bunch more commands, this time from FUZIX.
I also got the kernel to return an `errno` value back to
userland.

## Status - 19 Sept 2024

I've imported more programs, replaced `malloc()` with the Minix version and
added `readline()` support to the shell.

## Status - 17 Sept 2024

I have stage 1 up and running on the
[Rosco m68k Classic v2](https://github.com/rosco-m68k/rosco_m68k) SBC with the
bit-bang SD card. There are only a few programs in the userland at present:
`cal`, `cat`, `echo`, `ls`, `rm`, `sh`.

The kernel has these system calls:
`exit()`, `read()`, `write()`, `open()`, `close()`, `link()`, `unlink()`, `chdir()`,
`fstat()`, `dup()`, `mkdir()`, `spawn()`, `lseek()`, `time()`, `ioctl()`.

As there are no processes, `spawn()` acts like the Unix `exec()` and replaces the
current program, and `exit()` simply `spawn()s` the shell.

The next steps are to port more programs over from
[xv6-freebsd](https://github.com/DoctorWkt/xv6-freebsd) and
[Nine-E](https://github.com/DoctorWkt/Nine-E/).
