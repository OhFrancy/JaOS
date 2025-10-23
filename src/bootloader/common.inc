;
; eax = register value to print in hex
;
print_hex:
	push eax
	push ebx
	push ecx
	push edx

	mov ebx, eax

	mov cx, 8
	mov edx, ebx	; save this so we don't lose it when masking later
		
	.loop:
	rol edx, 4

	mov ebx, edx
	and ebx, 0xF	; mask out the top 12 bits, keep the low 4 that'll be processed
	
	cmp ebx, 10
	jl .num

	add ebx, 55	; 9 to 'A' ASCII table offset
	jmp .print_char

	.num:
	add ebx, '0'
	
	.print_char:
	mov eax, ebx
	mov ah, 0xE
	int 0x10

	loop .loop

	mov ah, 0xE
	mov al, 'h'
	int 0x10

	pop edx
	pop ecx
	pop ebx
	pop eax
	ret
;
; eax = register value to print in base 10 
;
print_dec:
	pusha
	
	xor edx, edx
	xor cx, cx
	mov ebx, 10		; base

.loop:
	inc cx			; save count of push operations
	div ebx			; n / 10 = eax
				; n % 10 = edx
	push edx		; we save the remainder for each operation, so that we can pop it and print it in the right order
	xor edx, edx
	test eax, eax		; when the quotient is 0, the remainder will be the first number starting from the left, so we are finished
	jz .print_loop
	jmp .loop

.print_loop:
	
	pop eax			
	add eax, '0'		

	mov ah, 0xE
	push cx
	int 0x10
	pop cx

	loop .print_loop	

	popa
	ret

;
; Read Disk Function (int 13h, 2)
; func params:
;    al    - sectors to read count
;    cx    - LBA
;    dl    - Drive number
;    es:bx - Buffer
disk_read:
     ; save registers that we will modify
     push ax
     push bx
     push cx
     push dx
     push di
	
     push es
     call lba_to_chs
     pop es

     ; this will be tried 3 times, as a precaution, the disk will be reset for every fail
     mov di, 3
.retry_loop:
     push es
    
     pop es

     mov ah, 0x2
     int 0x13                           ; clears carry flag if success, error otherwiseS
     jnc .done                          ; jump if carry cleared

     call reset_disk
     dec di

     test di, di
     jnz .retry_loop

.failed_all:
     mov si, msg_read_failure
     jmp error_reboot

.done:
     mov si, msg_read_success
     call print

     pop di
     pop dx
     pop ax				; restore LBA (cx) into AX
	
     call print_dec

     mov si, msg_ok
     call print

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
; Convertion LBA - CHS Function
; params:
;    cx             - LBA address
; returns:
;    cx (0-6 bits)  - Sector
;    cx (6-15 bits) - Cylinder
;    dh             - Head
lba_to_chs:
     push ax
     push bx

     mov bl, dl				; save this so we don't lose the drive number
     mov ax, cx

     
     xor dx, dx
     div WORD [bpb_sectors_per_track]   ;    ax = (LBA / SectorsPerTrack)
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

     mov dl, bl				; restore drive number
     pop bx
     pop ax
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
     mov ah, 0xE
     int 0x10
     jmp .loop

.done:
     pop si
     pop ax
     ret


