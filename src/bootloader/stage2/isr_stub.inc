[BITS 32]

extern isr_handler
extern irq_handler

global isr_stub
isr_stub:
	pushad
	mov ax, ds
	push	eax

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push esp
	call isr_handler
	pop  esp

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

     	popad
     	add esp, 0x8

	iret

global irq_stub
irq_stub:
	pusha
	mov ax, ds
	push	eax

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push esp
	call irq_handler
	pop esp
	
	mov al, 0x20
	out 0xA0, al
	out 0x20, al

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popa
	add esp, 0x8
	iret

%macro ISR_NO_ERR_CODE 1
	global isr%1
	isr%1:
		cli
		push byte 0
		push byte %1
		jmp isr_stub
%endmacro

%macro IRQ 2
	global irq%1
	irq%1:
		cli
		push byte 0
		push byte %2
		jmp irq_stub
%endmacro


%macro ISR_ERR_CODE 1
isr%+%1:
	push byte %1
	jmp isr_stub
%endmacro

ISR_NO_ERR_CODE 0		; Division by zero (DIV & IDIV)
ISR_NO_ERR_CODE 1		; Debug
ISR_NO_ERR_CODE 2		; External NM
ISR_NO_ERR_CODE 3		; Breakpoint
ISR_NO_ERR_CODE 4		; Overflow
ISR_NO_ERR_CODE 5		; Bound exceeded
ISR_NO_ERR_CODE 6		; Invalid Opcode
ISR_NO_ERR_CODE 7		; Device not available
ISR_ERR_CODE	 8		; Double fault
ISR_NO_ERR_CODE 9		; Coprocessor segment overrun
ISR_ERR_CODE	 10		; Invalid TSS
ISR_ERR_CODE	 11		; Segment not present
ISR_ERR_CODE	 12		; SS fault
ISR_ERR_CODE	 13		; GP
ISR_ERR_CODE    14		; Page fault
ISR_NO_ERR_CODE 15		; Intel Reserved
ISR_NO_ERR_CODE 16		; Math fault
ISR_ERR_CODE    17		; Alignment check
ISR_NO_ERR_CODE 18		; Machine check
ISR_NO_ERR_CODE 19	 	; SIMD Floating point exception
ISR_NO_ERR_CODE 20		; Virtualization exception
ISR_ERR_CODE	 21		; Control Protection exception

; Reserved 22-31
ISR_NO_ERR_CODE 22
ISR_NO_ERR_CODE 23
ISR_NO_ERR_CODE 24
ISR_NO_ERR_CODE 25
ISR_NO_ERR_CODE 26
ISR_NO_ERR_CODE 27
ISR_NO_ERR_CODE 28
ISR_NO_ERR_CODE 29
ISR_NO_ERR_CODE 30
ISR_NO_ERR_CODE 31

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

global isr_stub_table
isr_stub_table:
%assign i 0
%rep    32
    	dd isr%+i
%assign i i+1
%endrep

global irq_stub_table
irq_stub_table:
%assign i 0
%rep 	16
	dd irq%+i
%assign i i+1
%endrep
