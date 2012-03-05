#include <osconfig.h>

.section	.text

#ifdef x64

.globl win_setjmp, win_longjmp, nix_setjmp, nix_longjmp
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

nix_setjmp:
	leaq	8(%rsp),%rdx
	movq	(%rsp),%rax
	movq	%rbx,(%rdi)
	movq	%rbp,0x08(%rdi)
#	movq	%rsi,0x10(%rdi)
#	movq	%rdi,0x18(%rdi)
	movq	%r12,0x20(%rdi)
	movq	%r13,0x28(%rdi)
	movq	%r14,0x30(%rdi)
	movq	%r15,0x38(%rdi)
	movq	%rdx,0x40(%rdi)
	movq	%rax,0x48(%rdi)
	xorq	%rax,%rax
	ret

nix_longjmp:
	movq	(%rdi), %rbx
	movq	0x08(%rdi), %rbp
#	movq	0x10(%rdi), %rsi
#	movq	0x18(%rdi), %rdi
	movq	0x20(%rdi), %r12
	movq	0x28(%rdi), %r13
	movq	0x30(%rdi), %r14
	movq	0x38(%rdi), %r15
	movq	0x40(%rdi), %rsp
	movq	0x48(%rdi), %rax
	xchgq	%rsi, %rax
	jmp *%rsi

#else

.globl _win_setjmp, _win_longjmp, nix_setjmp, nix_longjmp
nix_setjmp:
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

nix_longjmp:
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

#endif

