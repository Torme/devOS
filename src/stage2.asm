bits	16
 
; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
; We are loaded at 0x500 (0x50:0)
 
org 0x500
 
jmp	main				; go to start
 
;*******************************************************
;	Preprocessor directives
;*******************************************************
 
%include "stdio.inc"			; basic i/o routines
%include "Gdt.inc"				; Gdt routines
%include "A20.inc"
 
;*******************************************************
;	Data Section
;*******************************************************
 
LoadingMsg db "Preparing to load devOS...", 0x0D, 0x0A, 0x00
WelcomeSpacing db 0x0A, 0x0A, 0x0A, 0x00
WelcomeTitle db  "-- ( dev OS v0.0.1 ) --", 0x0A, 0x00
WelcomeSubtitle db  "Think Better - Code Harder - Build Stronger", 0x0A, 0x00


;*******************************************************
;	STAGE 2 ENTRY POINT
;
;		-Store BIOS information
;		-Load Kernel
;		-Install GDT; go into protected mode (pmode)
;		-Jump to Stage 3
;*******************************************************
 
main:
 
	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;
 
	cli					; clear interrupts
	xor	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti					; enable interrupts
	
	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;
 
	mov	si, LoadingMsg
	call	Puts16
 
	;-------------------------------;
	;   Install our GDT		;
	;-------------------------------;
 
	call	InstallGDT		; install our GDT
 
	;-------------------------------;
	;   Enable A20			;
	;-------------------------------;

	call	EnableA20_KKbrd_Out

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;
 
EnterStage3:

	cli					; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax
 
	jmp	CODE_DESC:Stage3		; far jump to fix CS. Remember that the code selector is 0x8!
 
	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.
 
;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************
 
bits 32					; Welcome to the 32 bit world!
 
Stage3:
 
	;-------------------------------;
	;   Set registers		;
	;-------------------------------;
 
	mov		ax, 0x10		; set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 90000h		; stack begins from 90000h

	call	ClrScr32
	mov 	ebx, WelcomeSpacing
	call 	Puts32
	mov 	ebx, WelcomeTitle
	call  Ctrs32
	mov 	bl, 0x0A
	call 	Putch32
	mov 	ebx, WelcomeSubtitle
	call 	Ctrs32

;*******************************************************
;	Stop execution
;*******************************************************
 
STOP:
 
	cli
	hlt