[ORG 0x7C00]
[BITS 16]

;
; FAT 12 headers after overwrite
;

jmp short start
nop

bpb_oem:                      db "MSWIN4.1"            ; 8 bytes
bpb_bytes_per_sector:         dw 512
bpb_sectors_per_cluster:      db 1
bpb_reserved_sectors:         dw 1
bpb_fat_tables:               db 2
bpb_root_dir_entries:         dw 224
bpb_sectors:                  dw 2880
bpb_media_type:               db 0xF0
bpb_sectors_per_fat:          dw 9
bpb_sectors_per_track:        dw 18
bpb_heads:                    dw 2
bpb_hidden_sectors:           dd 0
bpb_large_sectors:            dd 0

ebr_drive_number:             db 0
                              db 0                     ; reserved
ebr_signature:                db 0x29
ebr_volume_id:                dd 0x10203040            ; random 4 bytes value
ebr_volume_label:             db "JAOS       "         ; 11 bytes, padded with spaces
ebr_sys_id:                   db "FAT12   "            ; 8 bytes, padded with spaces

;
; Bootcode start
;

start:
     jmp main

;
; Puts function, writes a string to the screen
; params:
;    si   - holds the string bytes
print:
     ; save registers that we'll modify
     push ax
     push si

.loop:
     lodsb                                        ; loads next character in al
     or al, al
     jz .done                                     ; if next character is null, stop printing and ret
     mov ah, 0xe
     int 0x10
     jmp .loop

.done:
     pop si
     pop ax
     ret

main:
     cld
     mov ax, 0x00
     mov ds, ax
     mov es, ax

	; set up a stack
     mov ss, ax
     mov sp, 0x7C00

	jmp load_rootdir


;
; FAT12 Loading files
;

;
; Loads the root dir sector
;

load_rootdir:
	; get the rootdir size in sectors
	; rootdir_size = (entries * 32) / bpb_bytes_per_sector
	mov ax, 32							; FAT entry size
	mul WORD [bpb_root_dir_entries]
	div WORD [bpb_bytes_per_sector]
	mov [rootdir_sectors], ax

	xor ax, ax

	; get the LBA of the rootdir
	; rootdir_start = (fat_count * sectors_per_fat) + reserved_sectors
	mov al, BYTE [bpb_fat_tables]
	mul BYTE [bpb_sectors_per_fat]			; bfr: dx:ax
	add ax, [bpb_reserved_sectors]

	; get the start of the first data cluster
	; first_cluster_start = rootdir_size + (fat_sector_size + reserved_sectors [register ax])

	mov WORD [datasector], ax
	mov bx, WORD [rootdir_sectors]
	add WORD [datasector], bx

	push ax
	xor ax, ax
	mov es, ax
	pop ax

	mov dl, [ebr_drive_number]
	mov cl, [rootdir_sectors]				; read all rootdir sectors
	mov bx, [rootdir_offset]					; load after the stage 1 bootloader sector
	call disk_read

	; start reading the rootdir entries and check for the filename
	mov cx, [bpb_root_dir_entries]
	mov di, [rootdir_offset]

	xor ax, ax
	mov es, ax
	.loop:
		push cx
		mov cx, 11						; file name length, check all char in the string
		mov si, second_stage_file
		push di
		repe cmpsb						; compare filename with byte at [es:di] (entry) till the end or non-match
		pop di
		je .loadfat						; found the address to find the file cluster
		pop cx
		add di, 32						; go to the next entry
		loop .loop

		mov si, msg_file_not_found
		jmp error_reboot

.loadfat:
	mov si, msg_file_found
	call print

	mov dx, WORD [di + 0x001A]				; get starting cluster ( + 26 )
	mov WORD [file_cluster], dx

	xor ax, ax
	mov es, ax							; set ES to 0

	mov ax, [bpb_reserved_sectors]			; read after the reserved boot sector
	mov bx, [fat_offset]					; save at 0x0000:0x2000
	mov cl, [bpb_sectors_per_fat]				; read all the FAT
	mov dl, BYTE [ebr_drive_number]

	call disk_read

	mov bx, [file_offset]					; load file at 0x0000:0x8000
	jmp .load_file

.load_file:
	mov ax, WORD [file_cluster]
	call cluster_to_lba						; convert file cluster to lba

	mov dl, [ebr_drive_number]
	mov cl, [bpb_sectors_per_cluster]			; read all sectors of the cluster

	call disk_read							; read the cluster and store it in our buffer

	; next cluster
	add bx, WORD [bpb_bytes_per_sector]		; increment the buffer by 512 bytes, save the next cluster (if any) in the next sector

	; test for the cluster
	mov ax, WORD [file_cluster]
	mov cx, ax
	mov dx, ax
	shr dx, 0x1							; cluster / 2
	add cx, dx							; cluster / 2 + cluster =  next cluster

	mov di, [fat_offset]					; location of FAT
	add di, cx							; index FAT
	mov dx, WORD [di]						; read two bytes
	test ax, 0x1
	jnz .odd

.even:
	and dx, 0xFFF
	jmp .done
.odd:
	shr dx, 0x4

.done:
	mov WORD [file_cluster], dx				; update cluster
	cmp dx, 0xFF0							; if valid cluster, go to the next one
	jb .load_file

	; jump to second stage
	jmp 0x0000:0x8000

;
; params:
;	ax	- cluster
; returns:
;	ax	-  lba
cluster_to_lba:
	push di
	sub ax, 0x2
	mul BYTE [bpb_sectors_per_cluster]
	add ax, WORD [datasector]
	pop di
	ret

;
; Convertion LBA - CHS Function
; params:
;    ax             - LBA address
; returns:
;    cx (0-6 bits)  - Sector
;    cx (6-15 bits) - Cylinder
;    dh             - Head
lba_to_chs:
     push ax
     push dx

     xor dx, dx
     div word [bpb_sectors_per_track]   ;    ax = (LBA / SectorsPerTrack)
                                        ;    dx = (LBA % SectorsPerTrack)
     inc dx                             ;    sector = (LBA % SectorsPerTrack) + 1
     mov cx, dx
     xor dx, dx
     div word [bpb_heads]               ;    ax = (LBA / SectorsPerTrack) / Heads = Cylinder
                                        ;    dx = (LBA / SectorsPerTrack) % Heads = Head
     mov dh, dl                         ;    Head
     mov ch, al
     shl ah, 6                          ; shift-left the 2 left bits to make them most significant
     or cl, ah                          ; OR with sector, to put the 2 MSb of cylinder at the end

     pop ax
     mov dl, al                         ; only restore dl, dh untouched
     pop ax
     ret

;
; Read Disk Function (int 13h, 2)
; params:
;    ax    - LBA Address
;    cl    - Numbers of sector to read
;    dl    - Drive number
;    es:bx - Buffer
disk_read:
     ; save registers that we will modify
     push ax
     push bx
     push cx
     push dx
     push di

     call lba_to_chs
; This will be retried 3 times, as a precaution, the disk will be reset for every fail

     mov di, 3
.retry_loop:
	pusha

     mov ah, 0x2
     stc
     int 0x13                           ; clears carry flag if success, error otherwise
     jnc .done                          ; jump if carry cleared

     popa
	call reset_disk
     dec di

     test di, di
     jnz .retry_loop

.failed_all:
	mov si, msg_read_failure
     jmp error_reboot

.done:
     popa
     pop di
     pop dx
     pop cx
     pop bx
     pop ax
     ret

;
; Reset Disk Function (int 13h, 0)
; params:
;    dl   - Drive number
reset_disk:
     pusha

     mov ah, 0x0
     int 0x13                           ; reset disk interrupt
	mov si, msg_read_failure
     jc error_reboot                    ; jmp if error

     popa
     ret

;
; Display error and reboot
; params:
;	si	- Message string
;
error_reboot:
	call print
     mov ah, 0x0
     int 0x16                           ; wait for key

     mov al, 0xFE
     out 0x64, al                       ; write FEh to port 0x64, triggering a reboot

; success/error messages
msg_read_failure	db "Disk read failed...", 10, 0
msg_file_not_found	db "File 'boot2.bin' not found...", 13, 10, 0
msg_file_found		db "File found, loading...", 13, 10, 0

; temporary variables
rootdir_sectors	dw 0x0
datasector		dw 0x0
file_cluster		dw 0x0

; loading offsets
fat_offset		dw 0x0500
rootdir_offset		dw 0x1000
file_offset		dw 0x8000

second_stage_file	db "BOOT2   BIN"	; 11 bytes, padded with spaces

times 510-($-$$) db 0                   ; fill sector with 0s

; Bootable signature
dw   0xAA55
