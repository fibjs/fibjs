#include <osconfig.h>

#ifdef MacOS
.section	__TEXT,__text
#else
.section	.text
#endif

#ifdef x64

.globl win_setjmp, win_longjmp, nix_setjmp, nix_longjmp, _nix_setjmp, _nix_longjmp
_nix_setjmp:
nix_setjmp:
    movq	%rcx,0x10(%rdi)
    movq    %rdi, %rcx
win_setjmp:
    movq	%rdx,0x18(%rcx)

	leaq	8(%rsp),%rdx
	movq	(%rsp),%rax
	movq	%rbp,(%rcx)
	movq	%rbx,0x08(%rcx)

	movq	%rsi,0x20(%rcx)
	movq	%rdi,0x28(%rcx)
	movq	%r12,0x30(%rcx)
	movq	%r13,0x38(%rcx)
	movq	%r14,0x40(%rcx)
	movq	%r15,0x48(%rcx)
	movq	%rdx,0x50(%rcx)
	movq	%rax,0x58(%rcx)
	movdqa	%xmm6, 0x60(%rcx)
	movdqa	%xmm7, 0x70(%rcx)
	movdqa	%xmm8, 0x80(%rcx)
	movdqa	%xmm9, 0x90(%rcx)
	movdqa	%xmm10, 0xa0(%rcx)
	movdqa	%xmm11, 0xb0(%rcx)
	movdqa	%xmm12, 0xc0(%rcx)
	movdqa	%xmm13, 0xd0(%rcx)
	movdqa	%xmm14, 0xe0(%rcx)
	movdqa	%xmm15, 0xf0(%rcx)
	xorq	%rax,%rax
	ret

_nix_longjmp:
nix_longjmp:
    movq    %rdi, %rcx
    movq    %rsi, %rdx
win_longjmp:
	movq	(%rcx), %rbp
	movq	0x08(%rcx), %rbx

	movq	0x20(%rcx), %rsi
	movq	0x28(%rcx), %rdi
	movq	0x30(%rcx), %r12
	movq	0x38(%rcx), %r13
	movq	0x40(%rcx), %r14
	movq	0x48(%rcx), %r15
	movq	0x50(%rcx), %rsp
	movq	0x58(%rcx), %rax
	movdqa	0x60(%rcx), %xmm6
	movdqa	0x70(%rcx), %xmm7
	movdqa	0x80(%rcx), %xmm8
	movdqa	0x90(%rcx), %xmm9
	movdqa	0xa0(%rcx), %xmm10
	movdqa	0xb0(%rcx), %xmm11
	movdqa	0xc0(%rcx), %xmm12
	movdqa	0xd0(%rcx), %xmm13
	movdqa	0xe0(%rcx), %xmm14
	movdqa	0xf0(%rcx), %xmm15
	xchgq	%rdx, %rax
	pushq %rdx
	movq	0x18(%rcx), %rdx
	movq	0x10(%rcx), %rcx
	ret

#else

.globl _win_setjmp, _win_longjmp, nix_setjmp, nix_longjmp, _nix_setjmp, _nix_longjmp
nix_setjmp:
_nix_setjmp:
_win_setjmp:
	leal	4(%esp), %edx
	movl	(%edx), %eax
	movl	%ebp, (%eax)
	movl	%ebx, 0x04(%eax)
	movl	%ecx, 0x08(%eax)
	movl	%esi, 0x0c(%eax)
	movl	%edi, 0x10(%eax)
	movl	%edx, 0x14(%eax)
	movl	(%esp), %edx
	movl	%edx, 0x18(%eax)
	xorl	%eax, %eax
	ret

nix_longjmp:
_nix_longjmp:
_win_longjmp:
	movl	0x4(%esp), %eax
	movl	0x8(%esp), %edx
	movl	(%eax), %ebp
	movl	0x04(%eax), %ebx
	movl	0x08(%eax), %ecx
	movl	0x0c(%eax), %esi
	movl	0x10(%eax), %edi
	movl	0x14(%eax), %esp
	movl	0x18(%eax), %eax
	xchgl	%edx, %eax
	jmp 	*%edx

#endif

