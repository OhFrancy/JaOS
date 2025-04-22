[BITS 32]

global _udiv64
_udiv64:
	push ebp
	mov     ebp, esp
	push    ebx
	push    esi
	push    edi

	mov     ecx, [ebp + 16]
	mov     ebx, [ebp + 20]

	test    ecx, ecx
	jnz     .divisor_ok
	.div_by_zero:
	mov     eax, 0xFFFFFFFF
	jmp     .cleanup

.divisor_ok:
	mov     eax, [ebp + 8]
	xor     edx, edx
	div     ecx

	test    ebx, ebx
	jz      .skip_store
	mov     [ebx + 4], eax

.skip_store:

	mov     eax, [ebp + 12]
	div     ecx
	test    ebx, ebx
	jz      .prepare_ret
	mov     [ebx], eax

.prepare_ret:
	mov     eax, edx

.cleanup:
	pop     edi
	pop     esi
	pop     ebx
	mov     esp, ebp
	pop     ebp
	ret
