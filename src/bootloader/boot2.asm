[ORG 0x8000]
[BITS 16]

start:
     JMP main

; Function to print a given value to an hex string
; params:
;	dx	- value
print_hex:
	pusha				; save all values to the stack
	mov cx, 4				; start the counter

.iterate_loop:
	dec cx

	mov ax,dx
	shr dx,4
	and ax,0xf			; get the last 4 bits

	mov bx, hex_str_out		; set bx to the memory address of our string
	add bx, 2				; skip '0x'
	add bx, cx			; add index

	cmp ax,0xa
	jl .letter			; if it's a number, set the value
	add byte [bx], 0x7		; If it's a letter, add 7
	jl .letter

.letter:
	add byte [bx], al		; Add the value of the byte to the char at bx

	cmp cx, 0				;
	je .done_hex_print		; if loop is done exit
	jmp .iterate_loop		; else continue

.done_hex_print:
	mov cx, hex_str_out		; print the string pointed to by bx
	call print

	popa
	ret

; Function to print a string
; params:
;	cx	- the string to print
print:
	push ax
	push bx
	push si
	xor bx, bx

	mov si, cx			; move the string to si to start printing
.print_loop:
     lodsb
     or al, al                ; if next char is null, done
     jz .done_print
     mov ah, 0xe
     int 0x10                 ; write character
     jmp .print_loop

.done_print:
	pop si
	pop bx
	pop ax
     ret

main:

     mov cx, msg_hello
     call print

	mov dx, 0x1234
	call print_hex

	cli
	hlt

msg_hello db "Hello World! From stage 2", 13, 10, 0
hex_str_out db "0x0000", 13, 10, 0
