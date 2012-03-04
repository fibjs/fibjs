.section	.text

.ifdef x86

.globl _win_setjmp, _win_longjmp
_win_setjmp:
	movl	(%esp), %ecx
	leal	4(%esp), %edx
	movl	(%edx), %eax
	movl	%ebx, (%eax)
	movl	%esi, 0x04(%eax)
	movl	%edi, 0x08(%eax)
	movl	%ebp, 0x0c(%eax)
	movl	%edx, 0x10(%eax)
	movl	%ecx, 0x14(%eax)
	xorl	%eax, %eax
	ret

_win_longjmp:
	movl	0x4(%esp), %ecx
	movl	0x8(%esp), %eax
	movl	(%ecx), %ebx
	movl	0x04(%ecx), %esi
	movl	0x08(%ecx), %edi
	movl	0x0c(%ecx), %ebp
	movl	0x10(%ecx), %esp
	movl	0x14(%ecx), %edx
	jmp 	*%edx

.else

.globl win_setjmp, win_longjmp, nix_setjmp, nix_longjmp
nix_setjmp:
win_setjmp:
	leaq	8(%rsp),%rdx
	movq	(%rsp),%rax
	movq	%rbx,(%rcx)
	movq	%rbp,0x08(%rcx)
	movq	%rsi,0x10(%rcx)
	movq	%rdi,0x18(%rcx)
	movq	%r12,0x20(%rcx)
	movq	%r13,0x28(%rcx)
	movq	%r14,0x30(%rcx)
	movq	%r15,0x38(%rcx)
	movq	%rdx,0x40(%rcx)
	movq	%rax,0x48(%rcx)
	xorq	%rax,%rax
	ret

nix_longjmp:
win_longjmp:
	movq	(%rcx), %rbx
	movq	0x08(%rcx), %rbp
	movq	0x10(%rcx), %rsi
	movq	0x18(%rcx), %rdi
	movq	0x20(%rcx), %r12
	movq	0x28(%rcx), %r13
	movq	0x30(%rcx), %r14
	movq	0x38(%rcx), %r15
	movq	0x40(%rcx), %rsp
	movq	0x48(%rcx), %rax
	xchgq	%rdx, %rax
	jmp *%rdx

.endif
