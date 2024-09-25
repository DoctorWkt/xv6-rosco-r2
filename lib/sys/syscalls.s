	section .text

; Perform system calls.
; For now, D0 holds the 32 bit result.
; Later, D1 holds the top 32 bits of a 64-bit
; result and A1 holds the errno value.
do_syscall:
	trap    #11			; Trap to the xv6 kernel
    	movem.l A1,errno		; Update errno from the syscall
    	movem.l (A7)+,D1/A1		; Restore two regs
    	rts

consputc::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #0,D1			; Syscall 0
	jmp	do_syscall

_exit::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #1,D1			; Syscall 1
	jmp	do_syscall

read::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #3,D1			; Syscall 3
	jmp	do_syscall

write::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #4,D1			; Syscall 4
	jmp	do_syscall

open::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #5,D1			; Syscall 5
	jmp	do_syscall

close::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #6,D1			; Syscall 6
	jmp	do_syscall

link::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #9,D1			; Syscall 9
	jmp	do_syscall

unlink::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #10,D1			; Syscall 10
	jmp	do_syscall

chdir::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #12,D1			; Syscall 12
	jmp	do_syscall

sys_fstat::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #13,D1			; Syscall 13
	jmp	do_syscall

dup::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #14,D1			; Syscall 14
	jmp	do_syscall

mkdir::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #15,D1			; Syscall 15
	jmp	do_syscall

spawn::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #16,D1			; Syscall 16
	jmp	do_syscall

consgetc::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #17,D1			; Syscall 17
	jmp	do_syscall

lseek::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #18,D1			; Syscall 18
	jmp	do_syscall

time::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #19,D1			; Syscall 19
	jmp	do_syscall

ioctl::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #20,D1			; Syscall 20
	jmp	do_syscall

stime::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #21,D1			; Syscall 21
	jmp	do_syscall

sleep::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #22,D1			; Syscall 22
	jmp	do_syscall

fchdir::
	movem.l D1/A1,-(A7)		; Save two regs
	move.l  #23,D1			; Syscall 23
	jmp	do_syscall
