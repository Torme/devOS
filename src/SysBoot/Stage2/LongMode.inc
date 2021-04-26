;*******************************************************
;
;	LongMode.inc
;		LongMode detection and enabling
;
;   https://wiki.osdev.org/Setting_Up_Long_Mode
;*******************************************************

%ifndef __LONGMODE_INC_INCLUDED__
%define __LONGMODE_INC_INCLUDED__

bits  32

MessageNoCPUID db "*** FATAL: No CPUID detected. Cannot setup Long Mode.", 0
MessageNoLongMode db "*** FATAL: LongMode not available.", 0

SetUpLongMode:
  call CPUIDDetection
  call LongModeDetection

  call SetupPAE
  call EnterInLongMode
  ret

	;-----------------------------------;
	; Check for Long Mode compatibility ;
	;-----------------------------------;

CPUIDDetection:
	; Check if CPUID is supported by attempting to flip the ID bit (bit 21) in
	; the FLAGS register. If we can flip it, CPUID is available.

	; Copy FLAGS in to EAX via stack
	pushfd
	pop eax
	mov ecx, eax ; Copy to ECX as well for comparing later on
	xor eax, 1 << 21    ; Flip the ID bit
	push eax    ; Copy EAX to FLAGS via the stack
	popfd
	pushfd    ; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
	pop eax

	; Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
	; back if it was ever flipped).
	push ecx
	popfd

	; Compare EAX and ECX. If they are equal then that means the bit wasn't
	; flipped, and CPUID isn't supported.
	xor eax, ecx
	jz .NoCPUID
	ret

  .NoCPUID:
    mov	ebx, MessageNoCPUID
    call	Puts32
    cli
    hlt

LongModeDetection:
	mov eax, 0x80000000    ; Set the A-register to 0x80000000.
	cpuid                  ; CPU identification.
	cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
	jb .NoLongMode
	mov eax, 0x80000001    ; Set the A-register to 0x80000001.
	cpuid                  ; CPU identification.
	test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
	jz .NoLongMode   
  ret

  .NoLongMode:
    mov	ebx, MessageNoLongMode
    call	Puts32
    cli
    hlt

SetupPAE:
  ; Disable paging
  mov eax, cr0                                   ; Set the A-register to control register 0.
  and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
  mov cr0, eax                                   ; Set control register 0 to the A-register.

  ; Clear tables
  mov edi, 0x1000    ; Set the destination index to 0x1000.
  mov cr3, edi       ; Set control register 3 to the destination index.
  xor eax, eax       ; Nullify the A-register.
  mov ecx, 4096      ; Set the C-register to 4096.
  rep stosd          ; Clear the memory.
  mov edi, cr3       ; Set the destination index to control register 3.

  ; Setup tables
  mov DWORD [edi], 0x2003      ; Set the uint32_t at the destination index to 0x2003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.
  mov DWORD [edi], 0x3003      ; Set the uint32_t at the destination index to 0x3003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.
  mov DWORD [edi], 0x4003      ; Set the uint32_t at the destination index to 0x4003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.

  ; Setup the 2 first Gb
  mov ebx, 0x00000003          ; Set the B-register to 0x00000003.
  mov ecx, 512                 ; Set the C-register to 512.
 
.SetEntry:
  mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
  add ebx, 0x1000              ; Add 0x1000 to the B-register.
  add edi, 8                   ; Add eight to the destination index.
  loop .SetEntry               ; Set the next entry.

  ; Enable PAE
  mov eax, cr4                 ; Set the A-register to control register 4.
  or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
  mov cr4, eax                 ; Set control register 4 to the A-register.

  ; PML5 Detection
  mov eax, 0x7                 ; You might want to check for page 7 first!
  xor ecx, ecx
  cpuid
  test ecx, (1<<16)
  jnz .5_level_paging
  ret

.5_level_paging:
  mov eax, cr4
  or eax, (1<<12) ;CR4.LA57
  mov cr4, eax
  ret

EnterInLongMode:
  ; set the LM-bit
  mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
  rdmsr                        ; Read from the model-specific register.
  or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
  wrmsr                        ; Write to the model-specific register.

  ; Enable Paging
  mov eax, cr0                 ; Set the A-register to control register 0.
  or eax, 1 << 31              ; Set the PG-bit, which is the 32nd bit (bit 31).
  mov cr0, eax                 ; Set control register 0 to the A-register.

%endif