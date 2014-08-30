
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

	mov	rcx, [rcx + 010h]
	push	rax
	xor		rax, rax

	ret

_TEXT   ENDS

END
