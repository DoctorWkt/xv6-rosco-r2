;------------------------------------------------------------
;                                  ___ ___ _
;  ___ ___ ___ ___ ___       _____|  _| . | |_
; |  _| . |_ -|  _| . |     |     | . | . | '_|
; |_| |___|___|___|___|_____|_|_|_|___|___|_,_|
;                     |_____|           stdlibs
;------------------------------------------------------------
; Copyright (c)2020-2021 Ross Bamford See top-level LICENSE.rosco_m68k
; for licence information.
;
; This is the initialization code. The loader jumps into this code after the
; "program" is received via serial.
;
; The first section is position independent, but will be loaded by the
; firmware loader at $00040000. This section copies the rest of the loaded 
; code to $2000, and then jumps to it.
;
; The second section (after RELOCATED_START) is executed next, based at $1000.
; This just calls out to __kinit to initialize .data and .bss, and then does a
; jump straight into kmain (the user program).
;
; All of this is depending on a bit of linker magic - see rosco_m68k_kernel.ld
; to see how that works.
;
; GCC global constructors/destructors (with attributes and 
; __cxa_atexit) are supported here, too.
;------------------------------------------------------------
;    include "../../shared/rosco_m68k_public.asm"

; Public equates for rosco_m68k firmware

; System Data Block (SDB) layout
SDB_MEMSIZE     equ     $414              ; Memory size (first block)

; Extension Function Pointer (EFP) table addresses
EFP_PROG_EXIT   equ     $490

    section .init

; NOTE: Loaded at $00040000 but init code is position-independent.

RELOC:                            ; position-independent load addr
    lea.l   RELOC(PC),A0          ; PC-rel source addr (load addr)
    lea.l   _init,A1              ; absolute dest addr (run addr)
    move.l  #_postinit_end,D0     ; init section absolute end addr
    sub.l   A1,D0                 ; subtract dest addr for init length
    lsr.l   #2,D0                 ; convert to long words
    subq.l  #1,D0                 ; subtract 1 for dbra
.INIT_LOOP:
    move.l  (A0)+,(A1)+           ; copy long word from source to dest
    dbra    D0,.INIT_LOOP         ; loop until end of postinit

    jmp     _postinit             ; jump to copied postinit at run addr

    section .postinit

POSTINIT:                         ; running from copied run addr location
    move.l  #_data_end,D0         ; absolute data end addr from linker
    sub.l   A1,D0                 ; subtract dest addr for code+data length
    lsr.l   #2,D0                 ; convert to long words
    move.l  D0,D1                 ; copy for outer loop
    swap    D1                    ; swap for 64K chunk count
    bra.b   .COPY_START           ; branch to loop test first

.COPY_LOOP:
    move.l  (A0)+,(A1)+           ; copy long word from source to dest
.COPY_START
    dbra    D0,.COPY_LOOP         ; inner loop
    dbra    D1,.COPY_LOOP         ; outer loop

_start::                          ; entrypoint at copied run addr location
    move.l  SDB_MEMSIZE,A7        ; reset stack to top of memory
 
    lea.l   __kinit,A0
    jsr     (A0)                  ; prepare C environment
    bsr.s   CALL_CTORS            ; Call global constructors
    lea.l   kmain,A0
    jsr     (A0)                  ; Fly user program, Fly!

exit::
    ;; If user code called exit directly, there'll be an exit code on the stack here...
    ;; Just ignore it, we don't care about it anyhow, but predec the stack to load the
    ;; null dso_handle anyway in case we fell through and the stack is empty...
    ;;
    move.l  #0,-(A7)              ; Call __cxa_finalize with NULL dso_handle
    lea.l   __cxa_finalize,A0
    jsr     (A0)
    add.l   #4,A7
    bsr.s   CALL_DTORS            ; Call global destructors
    move.l  EFP_PROG_EXIT,A0      ; And jump to PROG_EXIT
    jmp     (A0) 

CALL_CTORS:
    movem.l A2-A4,-(A7)

    move.l  #_ctors,A2
    move.l  #_ctors_end,A3

.LOOP
    sub.l   #4,A3

    cmp.l   A2,A3
    bcs.s   .DONE

    move.l  (A3),A4
    jsr     (A4)

    bra.s   .LOOP

.DONE
    movem.l (A7)+,A2-A4
    rts


CALL_DTORS:
    movem.l A2-A4,-(A7)

    move.l  #_dtors,A2
    move.l  #_dtors_end,A3

.LOOP
    cmp.l   A3,A2
    beq.s   .DONE

    move.l  (A2),A4
    jsr     (A4)

    add.l   #4,A2
    bra.s   .LOOP

.DONE
    movem.l (A7)+,A2-A4
    rts

    section .bss,bss
    align  4
