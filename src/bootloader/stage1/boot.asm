;
; Bootloader's first stage
;
;	Copyright (c) 2025 Francesco Lauro. All rights reserved.
;	SPDX-License-Identifier: MIT
;
[BITS 16]

jmp short _start
nop

%include "src/bootloader/bpb_params.inc"

global _start
_start:
	jmp main
	
%include "src/bootloader/common.inc"

main:
     	cli
	xor ax, ax
     	mov ds, ax
     	mov es, ax
     	mov ss, ax
     	mov sp, 0x7C00
	sti

	; load 8 sectors, that  will be the stage 1.5, after the bootsector and jump to it
	mov al, 8
	mov bx, [stage1_5_offset]
	mov cx, 2
	mov dl, [ebr_drive_number]
 
	call disk_read
	
	xor ax, ax
	mov es, ax
	jmp bx


msg_read_failure	db "Disk read... Fail", 10, 13, 0
msg_read_success	db "Disk read LBA ", 0
msg_ok			db " Ok", 10, 13, 0

stage1_5_offset		dw 0x8000
times 510-($-$$) db 0                   ; pad remaining bytes with 0sboot1
dw 0xAA55


