; Imported from NetBSD memset.S

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

memset::
	move.l	d2,-(sp)
	move.l	8(sp),a0		; destination
	move.l	16(sp),d1		; count
	move.b	15(sp),d2		; character

	; It isn't worth the overhead of aligning to {long}word boundries
	; if the string is too short.
	
	cmp.l	#15,d1
	blt	Lbzbyte

	clr.l	d0			; replicate byte to fill longword
	move.b	d2,d0
	move.l	d0,d2
	lsl.l	#8,d0
	or.l	d0,d2
	lsl.l	#8,d0 
	or.l	d0,d2
	lsl.l	#8,d0
	or.l	d0,d2

	; word align
	move.l	a0,d0
	btst	#0,d0			; if (dst & 1)
	beq	Lbzalgndw		; 
	move.b	d2,(a0)+		;	*(char *)dst++ = X
	subq.l	#1,d1			;	len--
	addq.l	#1,d0
Lbzalgndw:

Lbzlong:
	; set by longwords
	move.l	d1,d0
	lsr.l	#2,d0			; cnt = len / 4
	beq	Lbzbyte			; if (cnt)
	subq.l	#1,d0			;	set up for dbf
Lbzlloop:
	move.l	d2,(a0)+		;	clear longwords
	dbf	d0,Lbzlloop		;	till done
	clr.w	d0
	subq.l	#1,d0
	bcc	Lbzlloop
	and.l	#3,d1			;	len = 4
	beq	Lbzdone

	subq.l	#1,d1			; set up for dbf
Lbzbloop:
	move.b	d2,(a0)+		; set bytes
Lbzbyte:
	dbf	d1,Lbzbloop		; till done
Lbzdone:
	move.l	8(sp),d0		; return destination
	move.l	(sp)+,d2
	rts
