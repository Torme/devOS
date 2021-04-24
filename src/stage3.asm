
;*******************************************************
;
;	Stage3.asm
;		A basic 32 bit binary kernel running
;
;	OS Development Series
;*******************************************************

org	0x100000			; Kernel starts at 1 MB

bits	32				; 32 bit code

jmp	Stage3				; jump to entry point

%include "stdio.inc"

WelcomeSpacing db 0x0A, 0x0A, 0x0A, 0x00
WelcomeTitle db  "-- ( dev OS v0.0.1 ) --", 0x0A, 0x00
WelcomeSubtitle db  "Think Harder - Code Better - Build Faster - Be Stronger", 0x0A, 0x00

Stage3:

	;-------------------------------;
	;   Set registers		;
	;-------------------------------;

	mov	ax, 0x10		; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 90000h		; stack begins from 90000h

	;---------------------------------------;
	;   Clear screen and print success	;
	;---------------------------------------;

	call	ClrScr32
	mov 	ebx, WelcomeSpacing
	call 	Puts32
	mov 	ebx, WelcomeTitle
	call  Ctrs32
	mov 	bl, 0x0A
	call 	Putch32
	mov 	ebx, WelcomeSubtitle
	call 	Ctrs32

	;---------------------------------------;
	;   Stop execution			;
	;---------------------------------------;

	cli
	hlt
