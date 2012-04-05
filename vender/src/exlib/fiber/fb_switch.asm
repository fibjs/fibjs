
PUBLIC win_switch

_TEXT   SEGMENT

win_switch:
	mov	[rcx], rbp
	mov	[rcx + 008h], rbx

    mov	[rcx + 018h], rdx
	mov	[rcx + 020h], rsi
	mov	[rcx + 028h], rdi
	mov	[rcx + 030h], r12
	mov	[rcx + 038h], r13
	mov	[rcx + 040h], r14
	mov	[rcx + 048h], r15

	lea	rax, [rsp + 8]
	mov	[rcx + 050h], rax

	mov	rax, [rsp]
	mov	[rcx + 058h], rax

	movdqa	[rcx + 060h], xmm6
	movdqa	[rcx + 070h], xmm7
	movdqa	[rcx + 080h], xmm8
	movdqa	[rcx + 090h], xmm9
	movdqa	[rcx + 0a0h], xmm10
	movdqa	[rcx + 0b0h], xmm11
	movdqa	[rcx + 0c0h], xmm12
	movdqa	[rcx + 0d0h], xmm13
	movdqa	[rcx + 0e0h], xmm14
	movdqa	[rcx + 0f0h], xmm15

    mov	rcx, rdx

	mov	rbp, [rcx]
	mov	rbx, [rcx + 008h]

	mov	rdx, [rcx + 018h]
	mov	rsi, [rcx + 020h]
	mov	rdi, [rcx + 028h]
	mov	r12, [rcx + 030h]
	mov	r13, [rcx+ 038h]
	mov	r14, [rcx + 040h]
	mov	r15, [rcx + 048h]
	mov	rsp, [rcx + 050h]
	mov	rax, [rcx + 058h]
	movdqa	xmm6, [rcx + 060h]
	movdqa	xmm7, [rcx + 070h]
	movdqa	xmm8, [rcx + 080h]
	movdqa	xmm9, [rcx + 090h]
	movdqa	xmm10, [rcx + 0a0h]
	movdqa	xmm11, [rcx + 0b0h]
	movdqa	xmm12, [rcx + 0c0h]
	movdqa	xmm13, [rcx + 0d0h]
	movdqa	xmm14, [rcx + 0e0h]
	movdqa	xmm15, [rcx + 0f0h]
	mov	rcx, [rcx + 010h]
	push	rax
	xor		rax, rax

	ret

_TEXT   ENDS

END
