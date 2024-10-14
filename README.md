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

I'm finding the bit-bang SD card very slow. I also really don't want to have to put
in a relocating loader into the kernel. So I have designed an
[expansion board](hardware) with 1M of RAM and a
[CH375](https://www.electrodragon.com/product/ch375-module-reading-and-writing-u-diskusb-communicate/)
USB interface. It has a
[base register](https://en.wikipedia.org/wiki/Base_and_bounds) in front of the RAM, so that I can have multiple processes, but each one will think that they start at the same base address.

There are two Git branches for this project:

 - This branch, `ch375`, has the code that supports the above expansion board.
   In this version, the system has multiple processes running at any time.
 - The `main` branch has the code that supports the SD card. In that version,
   there is only one program running at any time.

## Running the System in an Emulator

If you want to try this out without the hardware, then build my
[r68k emulator](https://github.com/DoctorWkt/rosco_m68k/tree/wkt_r68k/code/tools/r68k).

In this repository, I have provided a copy of the `xv6` kernel binary: `kern/xv6.bin`, and a filesystem image: `fs.img`. Copy these to where you built the emulator. Now you can do:

```
$ ./r68k -U fs.img xv6.bin
```

and you will see:

```
Welcome to xv6
About to initialise the CH375
xv6 superblock: size 9000 nblocks 8949 ninodes 200
  nlog 20 logstart 2 inodestart 22 bmap start 48
$ ls -l
-rwxrwxrwx     1 root root     46 Wed Sep  4 23:49:10 README
drwxrwxrwx     1 root root    896 Mon Oct 14 21:38:24 bin
drwxrwxrwx     1 root root     64 Wed Sep  4 23:27:09 etc
-rwxrwxrwx     1 root root   6071 Tue Aug  8 01:30:40 roff_manual
$
```

## Building the System for the CH375 Device

I need to put more docs here, but for now ...

You will need the [rosco toolchain](https://rosco-m68k.com/docs/toolchain-installation)
installed on your system as well as `mtools` and `mkfs.vfat`. There are probably a few
more things needed - let me know what I've forgotten.

At the top level, do a `make` to build the kernel, the tools, the libraries and
the commands. This will build the `fs.img` xv6 filesystem image.

Yes, there are a heap of warnings. Over time, I will try to fix them.

Then, do a `make sdcard.img`. This creates the SD card image with a bootable copy
of the `xv6` kernel.

Now write `sdcard.img` to your SD card, write `fs.img` to your USB key, put each
in the correct socket, and boot your rosco board with the CH375 expansion board.

You should see something like:

```
                                 ___ ___ _   
 ___ ___ ___ ___ ___       _____|  _| . | |_ 
|  _| . |_ -|  _| . |     |     | . | . | '_|
|_| |___|___|___|___|_____|_|_|_|___|___|_,_|
                    |_____|      Classic 2.42

MC68010 CPU @ 10.0MHz with 14680064 bytes RAM
Initializing hard drives... No IDE interface found
Searching for boot media...
SD v2 card:
  Partition 1: Loading "/ROSCODE1.BIN"....
Loaded 16600 bytes in ~1 sec.

Welcome to xv6
About to initialise the CH375
xv6 superblock: size 9000 nblocks 8949 ninodes 200
  nlog 20 logstart 2 inodestart 22 bmap start 48
$ setdate
Enter new date (yyyy-mm-dd): 2024-10-14
Enter new time (hh:mm:ss): 11:07:00
New time is Mon Oct 14 11:07:00 2024

$ touch README
$ ls -l
-rwxrwxrwx     1 root root     46 Mon Oct 14 11:07:05 README
-rwxrwxrwx     1 root root  42497 Thu Jan  1 00:01:13 bar
drwxrwxrwx     1 root root    896 Mon Oct 14 00:49:48 bin
drwxrwxrwx     1 root root     64 Wed Sep  4 23:27:09 etc
-rwxrwxrwx     1 root root  42497 Thu Jan  1 00:00:43 foo
-rwxrwxrwx     1 root root   6071 Tue Aug  8 01:30:40 roff_manual
$
```

## Status - 14 Oct 2024

The PCBs have arrived and the CH375 device works fine. While waiting for the PCBs,
I got `fork()`, `exit()` and `wait()` working correctly in the emulator. They also
work in hardware with the memory on the PCB.

There is now an `init` process, and I've imported the `xv6` shell. So now the system
has proper working processes.

I've made a start with the code for `pipe()` but it's not working yet.

## Status - 5 Oct 2024

I have started the work in the `ch375` branch to add processes to the system.
There is still only one process, but I've brought back the `proc` structure
and I've begun the work to write the memory management code.

## Status - 1 Oct 2024

I've imported the KiCad design for my expansion RAM + base register + CH375 board
and done a bit of documentation on it. I've ordered PCBs and I'm waiting for them
to arrive so I can test if the board works.

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
