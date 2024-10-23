; Borrowed from NetBSD bcopy.S 

; Copyright (c) 1997 The NetBSD Foundation, Inc.
; All rights reserved.
;
; This code is derived from software contributed to The NetBSD Foundation
; by J.T. Conklin.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
; ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
; TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
; PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
; BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
 

; Copyright (c) 1990 The Regents of the University of California.
; All rights reserved.
;
; This code is derived from software contributed to Berkeley by
; the Systems Programming Group of the University of Utah Computer
; Science Department.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.
; 3. Neither the name of the University nor the names of its contributors
;    may be used to endorse or promote products derived from this software
;    without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
; OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
; HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
; LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
; OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
; SUCH DAMAGE.
 
memmove::
	move.l	4(sp),a1		; dest address
	move.l	8(sp),a0		; src address
	move.l	12(sp),d1		; count

	cmp.l	a1,a0			; src after dest?
	blt	Lbcback			; yes, must copy backwards

	; It isn't worth the overhead of aligning to {long}word boundries
	; if the string is too short.
	 
	cmp.l	#8,d1
	blt	Lbcfbyte

	; The 68010 cannot access a word or long on an odd boundary,
	; period.  If the source and the destination addresses aren't
	; of the same evenness, we're forced to do a bytewise copy.
	 
	move.l	a0,d0
	add.l	a1,d0
	btst	#0,d0
	bne	Lbcfbyte
	
	; word align 
	move.l	a1,d0
	btst	#0,d0		; if (dst & 1)
	beq	Lbcfalgndw	; 
	move.b	(a0)+,(a1)+	;	*(char *)dst++ = *(char *) src++
	subq.l	#1,d1		;	len--

Lbcfalgndw:
	; long word align 
	btst	#1,d0		; if (dst & 2)
	beq	Lbcfalgndl
	move.w	(a0)+,(a1)+	;	*(short *)dst++ = *(short *) dst++

Lbcfalgndl:
	; copy by 8 longwords 
	move.l	d1,d0
	lsr.l	#5,d0		; cnt = len / 32
	beq	Lbcflong	; if (cnt)
	and.l	#31,d1		;	len %= 32
	subq.l	#1,d0		;	set up for dbf

Lbcf32loop:
	move.l	(a0)+,(a1)+	;	copy 8 long words
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	dbf	d0,Lbcf32loop	;	till done
	clr.w	d0
	subq.l	#1,d0
	bcc	Lbcf32loop

Lbcflong:
	; copy by longwords 
	move.l	d1,d0
	lsr.l	#2,d0		; cnt = len / 4
	beq	Lbcfbyte	; if (cnt)
	subq.l	#1,d0		;	set up for dbf

Lbcflloop:
	move.l	(a0)+,(a1)+	;	copy longwords
	dbf	d0,Lbcflloop	;	til done
	and.l	#3,d1		;	len %= 4
	beq	Lbcdone

	subq.l	#1,d1		; set up for dbf

Lbcfbloop:
	move.b	(a0)+,(a1)+	; copy bytes

Lbcfbyte:
	dbf	d1,Lbcfbloop	; till done

Lbcdone:
	move.l	4(sp),d0	; dest address
	rts

Lbcback:
	add.l	d1,a0		; src pointer to end
	add.l	d1,a1		; dest pointer to end

	; It isn't worth the overhead of aligning to {long}word boundries
	; if the string is too short.
	 
	cmp.l	#8,d1
	blt	Lbcbbyte

	; The 68010 cannot access a word or long on an odd boundary,
	; period.  If the source and the destination addresses aren't
	; of the same evenness, we're forced to do a bytewise copy.
	 
	move.l	a0,d0
	add.l	a1,d0
	btst	#0,d0
	bne	Lbcbbyte
	
	; word align 
	move.l	a1,d0
	btst	#0,d0		; if (dst & 1)
	beq	Lbcbalgndw	; 
	move.b	-(a0),-(a1)	;	*(char *)dst-- = *(char *) src--
	subq.l	#1,d1		;	len--

Lbcbalgndw:
	; long word align 
	btst	#1,d0		; if (dst & 2)
	beq	Lbcbalgndl
	move.w	-(a0),-(a1)	;	*(short *)dst-- = *(short *) dst--
	subq.l	#2,d1		;	len -= 2

Lbcbalgndl:
	; copy by 8 longwords 
	move.l	d1,d0
	lsr.l	#5,d0		; cnt = len / 32
	beq	Lbcblong	; if (cnt)
	and.l	#31,d1		;	len %= 32
	subq.l	#1,d0		;	set up for dbf

Lbcb32loop:
	move.l	-(a0),-(a1)	;	copy 8 long words
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	move.l	-(a0),-(a1)
	dbf	d0,Lbcb32loop	;	till done
	clr.w	d0
	subq.l	#1,d0
	bcc	Lbcb32loop
	
Lbcblong:
	; copy by longwords 
	move.l	d1,d0
	lsr.l	#2,d0		; cnt = len / 4
	beq	Lbcbbyte	; if (cnt)
	subq.l	#1,d0		;	set up for dbf

Lbcblloop:
	move.l	-(a0),-(a1)	;	copy longwords
	dbf	d0,Lbcblloop	;	til done
	and.l	#3,d1		;	len %= 4
	beq	Lbcdone

	subq.l	#1,d1		; set up for dbf

Lbcbbloop:
	move.b	-(a0),-(a1)	; copy bytes

Lbcbbyte:
	dbf	d1,Lbcbbloop	; till done

	move.l	4(sp),d0	; dest address
	rts
