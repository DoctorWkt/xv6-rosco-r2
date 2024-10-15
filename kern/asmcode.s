; CH375 I/O addresses
CHDATARD  equ	$FFF001
CHDATAWR  equ	$FFF001
CHCMDWR   equ	$FFF003

; CH375 Commands
CMD_RESET_ALL           equ $05
CMD_SET_USB_MODE        equ $15
CMD_GET_STATUS		equ $22
CMD_RD_USB_DATA         equ $28
CMD_WR_USB_DATA         equ $2B
CMD_DISK_INIT           equ $51
CMD_DISK_SIZE           equ $53
CMD_DISK_READ           equ $54
CMD_DISK_RD_GO          equ $55
CMD_DISK_WRITE          equ $56
CMD_DISK_WR_GO          equ $57
CMD_DISK_READY          equ $59

; CH375 Status Results
USB_INT_SUCCESS         equ $14
USB_INT_CONNECT         equ $15
USB_INT_DISCONNECT      equ $16
USB_INT_DISK_READ       equ $1D
USB_INT_DISK_WRITE      equ $1E

; UART I/O addresses
DUART_SRA equ   $F00003
DUART_RBA equ   $F00007
DUART_TBA equ   $F00007
DUART_IMR equ	$F0000B
R_STOPCNTCMD equ $F0001F

; Address of the IRQ5
; and TRAP11 vectors
IRQ5_VECTOR   equ $74
TRAP11_VECTOR equ $AC
TICK_VECTOR   equ $114

; Address of the base register
BASE_REG      equ $FFE001

	section .text

; This comes from xosera_m68k_api.c
cpu_delay::
	move.l 4(A7),D0
	lsl.l  #8,D0
	add.l  D0,D0
L1:	subq.l #1,D0
	tst.l  D0
	bne.s  L1
	rts

; Start the 100Hz heartbeat
start_timer::
    move.b  #$08,DUART_IMR	; Unmask counter interrupt
    rts

; Increment the tick counter
tick_handler::
    move.l D0,-(A7)		; Save D0
    addq.b #1,tick_cntr		; Increment the tick counter
    cmpi.b #100,tick_cntr	; Is it 100?
    bne.s  L2			; No, skip
    clr.b tick_cntr		; Set the tick counter to zero
    addq.l #1,epoch_time	; and increment the epoch time
L2:
    move.b R_STOPCNTCMD,D0	; Clear the interrupt
    move.l (A7)+,D0		; Restore D0
    rte

; This is the interrupt handler for the CH375.
; Send a CMD_GET_STATUS to the device,
; read a byte of data and save it in the
; CH375_STATUS location.
irq5_handler::
	move.b #CMD_GET_STATUS,CHCMDWR
	move.b CHDATARD,CH375_STATUS
	rte

; Install the IRQ5, tick and system call handler,
; put a dummy value in the CH375_STATUS byte,
; start the heartbeat and enable interrupts.
irq5_install::
	move.l #irq5_handler,IRQ5_VECTOR
	move.l #tick_handler,TICK_VECTOR
	move.l #SYSCALL_HANDLER,TRAP11_VECTOR
	move.b #$FF,CH375_STATUS
	jsr    start_timer
	and.w  #$F0FF,SR		; Enable all interrupts
	rts

; Write the given command to the CH375.
; Clear the CH375_STATUS beforehand by
; putting a dummy value there.
send_ch375_cmd::
	move.b #$FF,CH375_STATUS
	move.b 7(A7),D0
	move.b D0,CHCMDWR
	rts

; Write the given data to the CH375.
send_ch375_data::
	move.b 7(A7),D0
	move.b D0,CHDATAWR
	rts

; Read data from the CH375
read_ch375_data::
	move.b CHDATARD,D0
	rts

; Get the CH375 status from the
; CH375_STATUS memory location.
; Loop until it is not $FF
get_ch375_status::
        move.b CH375_STATUS,D0
        cmpi.b #$FF,D0
        beq    get_ch375_status
        rts

; Given a pointer to a 512-byte buffer and an
; LBA number, read the block from the CH375 into
; the buffer. Return 1 on success, 0 otherwise.
;
; unsigned char read_block(unsigned char *buf, uint lba)
;
read_block::

	; Check that buf isn't NULL
	movea.l	4(SP),A0
	cmp.l	#0,4(SP)
	beq.w	readfail

	; Send the disk read command followed by the LBA
	; in little-endian format, then ask for one block.
	move.b	#$FF,CH375_STATUS
	move.b	#CMD_DISK_READ,CHCMDWR
	move.b	11(SP),CHDATAWR
	move.b	10(SP),CHDATAWR
	move.b	9(SP),CHDATAWR
	move.b	8(SP),CHDATAWR
	move.b	#1,CHDATAWR

	; Loop eight times reading in
	; 64 bytes of data each time.
	moveq.l	#8,D1
READL1:

	; Get the status, ensure that
	; it is USB_INT_DISK_READ
	jsr	get_ch375_status
	cmpi.b	#USB_INT_DISK_READ,D0
	bne.s	readfail

	; Send the command to read the data,
	; get back the number of bytes to read.
 	; Divide it by eight to match the loop
	; unrolling below (this assumes the count
	; is a multiple of eight).
	move.b	#$FF,CH375_STATUS
	move.b	#CMD_RD_USB_DATA,CHCMDWR
	clr.l	D0
	move.b	CHDATARD,D0
	lsr.l	#3,D0

	; Loop cnt times reading data
READL2:
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	move.b  CHDATARD,(A0)+
	subi.l	#1,D0
	bne.s	READL2

	; After cnt bytes, tell the CH375
	; to get the next set of data
	; and loop back
	move.b	#$FF,CH375_STATUS
	move.b	#CMD_DISK_RD_GO,CHCMDWR
	subi.l	#1,D1
	bne.s	READL1

	; Get the status after reading the block
	jsr	get_ch375_status
	cmpi.b	#USB_INT_SUCCESS,D0
	bne.s	readfail

	; Return 1 on success, 0 on failure
readok:
	moveq.l	#1,d0
	rts

readfail:
	moveq.l	#0,d0
	rts

; Given a pointer to a 512-byte buffer and an
; LBA number, write the block from the buffer
; to the CH375. Return 1 on success, 0 otherwise.
;
; unsigned char write_block(unsigned char *buf, uint lba)
;
write_block::

	; Check that buf isn't NULL
	movea.l	4(SP),A0
	cmp.l	#0,4(SP)
	beq.w	writefail

	; Send the disk write command followed by the LBA in
	; little-endian format, then ask to send one block.
	move.b	#$FF,CH375_STATUS
	move.b	#CMD_DISK_WRITE,CHCMDWR
	move.b	11(SP),CHDATAWR
	move.b	10(SP),CHDATAWR
	move.b	9(SP),CHDATAWR
	move.b	8(SP),CHDATAWR
	move.b	#1,CHDATAWR

	; Loop eight times writing out
	; 64 bytes of data each time.
	moveq.l	#8,D1
WRITEL1:

	; Get the status, ensure that
	; it is USB_INT_DISK_WRITE
	jsr	get_ch375_status
	cmpi.b	#USB_INT_DISK_WRITE,D0
	bne.s	writefail

	; Send the command to write the data
	; along with the count. Then set D0
	; to 8 for the loop: it really is 64
	; but we do loop unrolling.
	move.b	#$FF,CH375_STATUS
	move.b	#CMD_WR_USB_DATA,CHCMDWR
	move.b	#64,CHDATAWR
	moveq.l	#8,D0

	; Loop 8 times writing data
WRITEL2:
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	move.b  (A0)+,CHDATAWR
	subi.l	#1,D0
	bne.s	WRITEL2

	; After 64 bytes, tell the CH375 to
	; get ready for the next set of data
	; and loop back
	move.b	#$FF,CH375_STATUS
	move.b	#CMD_DISK_WR_GO,CHCMDWR
	subi.l	#1,D1
	bne.s	WRITEL1

	; Get the status after writing the block
	jsr	get_ch375_status
	cmpi.b	#USB_INT_SUCCESS,D0
	bne.s	writefail

	; Return 1 on success, 0 on failure
writeok:
	moveq.l	#1,d0
	rts

writefail:
	moveq.l	#0,d0
	rts

; Print byte to UART A.
consputc::
	move.b 7(A7),D0
PUTC_WAIT:
	btst.b  #3,DUART_SRA
	beq.s   PUTC_WAIT
	move.b  D0,DUART_TBA
	rts

; Read character from UART A
consgetc::
	clr.l	D0		; Clear all of D0
.BUSYLOOP
	btst.b  #0,DUART_SRA	; Loop until there is
	beq.s   .BUSYLOOP	; a character.
	move.b  DUART_RBA,D0	; Get it into D0

    	cmpi.b	#13,D0		; Is it CR?
	bne.s	L3		; No
	move.b	#10,D0		; Yes, convert to LF (newline)

L3:
	cmp.b	#1,DUART_ECHO_A	; Should we echo it?
	bne.s	.NOECHO		; No, skip
	jsr	PUTC_WAIT	; Yes, echo it
.NOECHO
	rts

; Set the base register to the byte argument
setbasereg::
	move.b 7(A7),BASE_REG
	rts

; Disable all interrupts
cli::
	or.w	#$0700,SR
	rts

; Enable all interrupts
sti::
	and.w	#$F0FF,SR
	rts

; Return the current stack pointer,
; compensating for the jsr here
getsp::
	move.l	a7,d0
	addq.l	#4,d0
	rts

; saveregs() gets called to set up a newly-forked
; child's registers by copying the parent's registers.
;
; void saveregs(struct context *old)
saveregs::
	move.l	  4(a7),a0		; Get the old context pointer
	movem.l   d2-d7/a2-a6,(a0)	; Save the old registers
	rts

; swtch() takes context pointers for the old and new processes, as well
; as the new base register value. swtch() saves all the registers in the
; old context, restores the registers from the new context and sets the
; base register. It then returns to the new PC.
;
; void swtch(struct context *old, struct context *new, int basereg)
swtch::
	move.l	  4(a7),a0		; Get the old context pointer,
	move.l	  8(a7),a1		; the new context pointer
	move.l	  12(a7),d0		; and the base register.
	movem.l   d2-d7/a2-a7,(a0)	; Save the old registers
	move.l	  (a7),48(a0)		; and the old return address.
	movem.l   (a1),d2-d7/a2-a7	; Get the new registers
	move.b    d0,BASE_REG		; Change the address space
	move.l	  48(a1),(a7)		; Make the new PC the return address
	rts				; and return

; The system call trap handler.
; D1 holds the system call number.
SYSCALL_HANDLER::
	cmp.l   #25,D1		; Is it a valid syscall number?
	bhi.s	.EPILOGUE	; No, so return now

	move.l	$1C(A7),-(A7)	; Copy three original argument
	move.l	$1C(A7),-(A7)	; values below the trap frame
	move.l	$1C(A7),-(A7)

	add.l   D1,D1		; Multiply D1 by four
	add.l   D1,D1		; Find the address of the function
	move.l  .SYSTABLE(PC,D1),A1
	jsr     (A1)		; and jump to the function
.EPILOGUE
	add.l	#$C,A7		; Remove the copied arguments,
	move.l  errno,A1	; copy errno into A1
	rte			; and return to userland

.SYSTABLE:
	dc.l	consputc	;  0 = consputc, temporarily
	dc.l	sys_exit	;  1 = exit
	dc.l	sys_brk		;  2 = brk
	dc.l	sys_read	;  3 = read
	dc.l	sys_write	;  4 = write
	dc.l	sys_open	;  5 = open
	dc.l	sys_close	;  6 = close
	dc.l	sys_wait	;  7 = wait
	dc.l	sys_fork	;  8 = fork
	dc.l	sys_link	;  9 = link
	dc.l	sys_unlink	; 10 = unlink
	dc.l	sys_sbrk	; 11 = sbrk
	dc.l	sys_chdir	; 12 = chdir
	dc.l	sys_fstat	; 13 = fstat
	dc.l	sys_dup		; 14 = dup
	dc.l	sys_mkdir	; 15 = mkdir
	dc.l	sys_exec	; 16 = exec
	dc.l	consgetc	; 17 = consgetc, temporarily
	dc.l	sys_lseek	; 18 = lseek
	dc.l	sys_time	; 19 = time
	dc.l	sys_ioctl	; 20 = ioctl
	dc.l	sys_stime	; 21 = stime
	dc.l	sys_sleep	; 22 = sleep
	dc.l	sys_fchdir	; 23 = fchdir
	dc.l	sys_utime	; 24 = utime
	dc.l	sys_pipe	; 25 = pipe

.NULLSYS:
	rts
