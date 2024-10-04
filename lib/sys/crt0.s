; This is the xv6 userland initialization code.
; Some of it is borrowed from the rosco_m68k init.S file.

	section .init

_start::                          ; entrypoint
				  ; The a.ouh header looks like this
    jmp      _start2		  ; uint16_t: magic number $600C
    dc.w     $0000		  ; uint16_t: padding
    dc.l     _code_end		  ; uint32_t: end of machine code
    dc.l     _data_end		  ; uint32_t: end of initialised data
    dc.l     _bss_end		  ; uint32_t: end of BSS
    dc.l     $10000		  ; uint32_t: total size of process in bytes
_start2:
    lea.l   __kinit,A0
    jsr     (A0)                  ; prepare C environment
    lea.l   main,A0
    jsr     (A0)                  ; Run the program

    jsr exit			  ; Then call exit()
