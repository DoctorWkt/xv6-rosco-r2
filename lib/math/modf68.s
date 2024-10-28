.sect .text;.sect .rom;.sect .data;.sect .bss
.define _modf
.extern _modf
.sect .text
_modf:
tst.b -40(sp)
link	a6,#-0
move.l 8+4(a6),-(sp)
move.l 8(a6),-(sp)
clr.l	-(sp)
move.w #1,-(sp)
move.w #2,-(sp)
jsr .cif8
move.l sp,-(sp)
jsr .fif8
add.l #4,sp
move.l 16(a6), a1
move.l #3,d2
1:
move.w (sp)+,(a1)+
dbf d2, 1b
move.l (sp)+,d0
move.l (sp)+,d1
unlk a6
rts
