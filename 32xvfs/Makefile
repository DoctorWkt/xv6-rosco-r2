#CFLAGS= -m32 -I ../include -Wall -Wextra -Werror
CFLAGS= -m32 -I ../include -Wall
CFLAGS += -g

usertests: usertests.o fs.img libxv6fs.a
	cc -o usertests -m32 -g usertests.o libxv6fs.a

try: try.o fs.img libxv6fs.a
	cc -o try -g try.o libxv6fs.a

ls: ls.o fs.img libxv6fs.a
	cc -o ls -g ls.o libxv6fs.a

mkdir: mkdir.o fs.img libxv6fs.a
	cc -o mkdir -g mkdir.o libxv6fs.a

catinto: catinto.o fs.img libxv6fs.a
	cc -o catinto -g catinto.o libxv6fs.a

cat: cat.o fs.img libxv6fs.a
	cc -o cat -g cat.o libxv6fs.a

libxv6fs.a: blk.o bio.o file.o fs.o log.o sysfile.o
	ar cq libxv6fs.a blk.o bio.o file.o fs.o log.o sysfile.o
	ranlib libxv6fs.a

mkfs: mkfs.c
	cc -o mkfs $(CFLAGS) mkfs.c

fs.img:
	echo y | co -l fs.img

mk_fs.img: mkfs
	echo hello > echo
	./mkfs fs.img *.c echo

fred: cprintf.c blk.c bio.c log.c fs.c fred.c romfuncs.o big.img
	cfm -o fred cprintf.c blk.c bio.c log.c fs.c fred.c romfuncs.o

big.img:
	echo y | co -l big.img

romfuncs.o: romfuncs.s
	lwasm -f obj -o romfuncs.o romfuncs.s

clean:
	rm -f *.o mkfs *.img try fred libxv6fs.a ls mkdir catinto cat usertests
	rm -f echo
