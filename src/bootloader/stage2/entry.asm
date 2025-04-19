; Entry point for the bootloader second's stage
;
;	Copyright (c) 2025 Francesco Lauro. All rights reserved.
;	SPDX-License-Identifier: MIT
;

[BITS 16]

section .text
global _start

jmp _start

reboot:
.loop:
	xor al, al
	in al, 0x64
	test al, 2
	jnz .loop

	; wait for a key press
	mov al, 0x0
	int 0x16

	; reboot
	mov al, 0xFE
	out 0x64, al

; Function to print a string, real mode
; params:
;	cx	- the string to print
print_rmode:
	push ax
	push bx
	push si
	xor bx, bx

	mov si, cx					; move the string to si to start printing
.print_loop:
     lodsb
     or al, al						; if next char is null, done
     jz .done_print
     mov ah, 0xe
     int 0x10						; write character
     jmp .print_loop

.done_print:
	pop si
	pop bx
	pop ax
     ret

; build the global descriptor table
gdt:
gdt_null:
	dq 0
gdt_code:
	dw 0xFFFF						; set segment limit to the maximum
	dw 0x0000						; base address to 0

     db 0x00						; bits 0-7, base
	db 0x9A						; set the type to readable, non-conforming, highest privilege and present
	db 0xCF						; attributes and granularity, multiply limit by 4kB to get 4GB
	db 0x00						; last bits to the base address

gdt_data:
	; same as code segment
	dw 0xFFFF
	dw 0x0000
	db 0x00

	db 0x92						; only change 4th bit because we are setting the data segment now
	db 0xCF
	db 0

gdt_end:
gdt_descriptor:
	dw gdt_end - gdt - 1			; size of gdt
	dd gdt						; location in memory

_start:
	mov cx, msg_correct_switch
	call print_rmode

	extern main
	call main


enable_a20:
	in al, 0x92					; read from memory
	test al, 2					; check if A20 is already enabled
	jnz .alr_on

	or al, 2						; if not, enable it by setting bit 0
	and al, 0xFE					; only clear bit 0
	out 0x92, al					; write back to memory

	; check if it was enabled correctly
	in al, 0x92
	test al, 2
	jnz .alr_on

	; if not, throw an error and reboot
	mov cx, msg_failed_a20
	call print_rmode
	jmp reboot

.alr_on:
	xor ax, ax
	mov ds, ax

	cli
	lgdt [gdt_descriptor]

	; Enter protected mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 0x0008:enter_pmode		; jump to the first segment identifier

[BITS 32]
enter_pmode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov esp, 0x70000

	mov edi, 0xB8000
	mov ecx, 400
	mov eax, 0x07200720

	cld
	rep stosd

section .rodata
	msg_correct_switch  db "Switched execution to stage 2...", 13, 10, 0
	msg_failed_a20	     db "Failed to activate A20 line, press a key to reboot", 13, 10, 0
