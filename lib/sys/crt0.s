; This is the xv6 userland initialization code.
; A lot of it is borrowed from the rosco_m68k init.S file,
; so it's under the rosco_m68k license.

	section .init

_start::                          ; entrypoint
    lea.l   __kinit,A0
    jsr     (A0)                  ; prepare C environment
    ; bsr.s   CALL_CTORS            ; Call global constructors
    lea.l   main,A0
    jsr     (A0)                  ; Run the program

    jsr exit			  ; Then call exit()
