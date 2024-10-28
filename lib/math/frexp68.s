.sect .text;.sect .rom;.sect .data;.sect .bss
.define _frexp
.extern _frexp
.sect .text
_frexp:
tst.b -40(sp)
link	a6,#-0
move.l 8+4(a6),-(sp)
move.l 8(a6),-(sp)
move.l sp,a0
add.l #-2,a0
move.l a0,-(sp)
jsr .fef8
add.l #2,sp
move.w (sp)+,d2
move.l 16(a6), a0
move.w d2, (a0)
move.l (sp)+,d0
move.l (sp)+,d1
unlk a6
rts
