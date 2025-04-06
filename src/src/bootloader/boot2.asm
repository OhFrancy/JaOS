org 0x8000
bits 16

start:
     jmp main

; params: si
puts:
     lodsb
     or al, al                ; if next char is null, done
     jz .done
     mov ah, 0xe
     int 0x10                 ; write character
     jmp puts

.done:
     ret

main:
     mov si, msg_hello
     call puts

	cli
	hlt

msg_hello db "Hello World! From stage 2", 13, 10, 0

