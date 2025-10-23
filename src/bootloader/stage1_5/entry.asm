;
; Bootloader's mid-stage, acts as a loader for the second stage
;
;	Copyright (c) 2025 Francesco Lauro. All rights reserved.
;	SPDX-License-Identifier: MIT
;
[BITS 16]

global _start
_start:	
	cli
    	xor ax, ax
    	mov ds, ax
    	mov es, ax
    	mov ss, ax
    	mov sp, 0x7C00
    	sti

	mov si, msg_stage1_5_success
	call print
	
	call load_rootdir
	call search_file
	
	mov cx, 0
	mov es, cx
	mov bx, [stage2_offset]
	call read_clusterchain

	jmp [stage2_offset]

	

%include "src/bootloader/common.inc"

;
; Loads the rootdirectory in to memory at the offset in [rootdir_offset], should be called only once
;
; returns
; ax = root directory size in sectors
load_rootdir:
	push bx
	push cx
	push dx
	
	; AX = fat_size * fat_tables
	movzx eax, WORD [fat32_sectors_per_fat]
	movzx ecx, BYTE [bpb_fat_count]
	mul ecx
	
	; first_data_sector = reserved_sectors + AX
	movzx ecx, WORD [bpb_reserved_sectors]
	add eax, ecx
	mov [first_data_sector], eax
	
	xor ax, ax
	mov es, ax
	mov bx, [rootdir_offset]
	mov ax, [fat32_rootdir_cluster]			; first rootdir cluster	
	
	call read_clusterchain
	
	pop dx
	pop cx
	pop bx
	ret
	
;
; ax = first cluster
; es:bx = offset where the cluster chain will be loaded 
;
; returns
; ax    = count of sectors read
;
read_clusterchain:
	push bx
	push cx
	push dx

	mov [current_cluster], ax
	mov ax, 0

.clusterchain_loop:
	inc ax
	push ax
	; read the current cluster before checking for the next one
	mov ax, [current_cluster]
	
	call cluster_to_lba

	mov cx, ax
	xor ax, ax
	mov al, 1
	mov dl, [ebr_drive_number]
	
	call disk_read
	
	add bx, [bpb_bytes_per_sector]

	mov ax, [current_cluster]
	call get_next_cluster

	cmp eax, 0x0FFFFFF8
	jge .done

	mov [current_cluster], ax
	pop ax
	jmp .clusterchain_loop

.done:
	pop ax
	pop dx
	pop cx
	pop bx
     	ret
;
; searches for [file_name]
; params:
; ax    = root directory size in sectors, so that we read the right amount of entries
; es:bx = where to start reading
;
; returns:
; ax = file cluster if found
search_file:
	push ebx
	push ecx
	push edx
	push edi

	xor dx, dx
	mov si, file_name
	push si
	
	push bx
	mov bx, ax
	xor eax, eax
	mov ax, bx
	
	; knowing the amount of sectors, count of entries to read = ( (sectors_cnt * bytes_per_sector) / entry_bytes_size )
	mul WORD [bpb_bytes_per_sector]
	
	shl edx, 16
	or eax, edx

	mov cx, ax
	pop bx				; restore starting address 
	mov di, bx
.search_loop:
	pop si
	push si
	push cx				; save CX as we'll use it both for checking the file and looping in 'search_loop'
	mov cx, 11			; filename is 11 bytes
	
	push di
	repe cmpsb			; we need BX in DI because repe uses es:di
	pop di
	je .found
	pop cx
	add di, [bpb_rootdir_entry_size]

	cmp cx, 0
	jnz .search_loop		

.not_found:
	mov si, msg_file_not_found
	jmp error_reboot
.found:
	mov ax, [di + 26]		; 26th byte of the entry = starting cluster
	
	pop si
	pop cx
	pop edi
	pop edx
	pop ecx
	pop ebx
	ret

;
; params:
;	ax	- cluster
; returns:
;	ax	-  lba
cluster_to_lba:
	push dx
	push bx
	xor dx, dx
	sub ax, 0x2
	
	movzx bx, BYTE [bpb_sectors_per_cluster]
	mul bx
	add eax, DWORD [first_data_sector]
	pop bx
	pop dx
	ret
;
; params:
;	eax = current cluster
; returns
;	eax = next cluster value | >= 0x0FFFFFF8 if there is no next cluster (chain end)
get_next_cluster:
	push bx
	push dx

	; cluster offset in the FAT = cluster * 4 
	mov bx, ax
	shl bx, 2
	
	xor dx, dx
	mov ax, bx
	div WORD [bpb_bytes_per_sector]
	; ( cluster offset / bytes per sector ) + reserved sectors = fat sector = ax
	; cluster offset % bytes per sector = fat offset = dx
	
	add ax, WORD [bpb_reserved_sectors]
	
	mov di, dx			; save the offset for when we need to index later
	push di

	mov cx, ax			; LBA = fat sector = cx
	xor ax, ax				
	mov es, ax 
	mov bx, fat_sector_bfr
	mov al, 1			

	xor dx, dx
	mov dl, [ebr_drive_number]
	call disk_read

	pop di
	mov eax, [fat_sector_bfr + di]
	
	pop dx
	pop bx
	ret


msg_read_failure	db "Disk read... Fail", 10, 13, 0
msg_read_success	db "Disk read LBA ", 0
msg_ok			db " Ok", 10, 13, 0
msg_file_not_found	db "File not found in root directory."

file_name		db "LOADER  BIN"	; 11 bytes, padded with spaces
msg_stage1_5_success 	db "Stage 1.5 jump... Ok", 13, 10, 0
cluster_reading_done	db "Cluster chain reading done successfully", 13, 10, 0

; temporary variables
file_cluster		dw 0x0
current_cluster		dw 0x0
first_data_sector	dd 0x0

; loading offset
rootdir_offset		dw 0x1000
stage2_offset		dw 0xA000

fat_sector_bfr:		times 512 db 0
bpb_rootdir_entry_size	    db 32

%include "src/bootloader/bpb_params.inc"
