#include <osconfig.h>

#ifdef MacOS
.section	__TEXT,__text
#else
.section	.text
#endif

#ifdef x64

.globl win_switch, nix_switch, _nix_switch

_nix_switch:
nix_switch:
    movq	%rcx,0x10(%rdi)
    movq    %rdi, %rcx
win_switch:
	movq	%rbp,(%rcx)
	movq	%rbx,0x08(%rcx)

    movq	%rdx,0x18(%rcx)
	movq	%rsi,0x20(%rcx)
	movq	%rdi,0x28(%rcx)
	movq	%r12,0x30(%rcx)
	movq	%r13,0x38(%rcx)
	movq	%r14,0x40(%rcx)
	movq	%r15,0x48(%rcx)

	leaq	8(%rsp),%rax
	movq	%rax,0x50(%rcx)

	movq	(%rsp),%rax
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

#ifdef _WIN32
    movq	%rdx, %rcx
#else
    movq	%rsi, %rcx
#endif

	movq	(%rcx), %rbp
	movq	0x08(%rcx), %rbx

	movq	0x18(%rcx), %rdx
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
	movq	0x10(%rcx), %rcx
	pushq   %rax
	xorq	%rax,%rax
	ret

#else

.globl _win_switch, nix_switch, _nix_switch

nix_switch:
_nix_switch:
_win_switch:
	movl	0x4(%esp), %eax
	movl	%ebp, (%eax)
	movl	%ebx, 0x04(%eax)
	movl	%ecx, 0x08(%eax)
	movl	%edx, 0x0c(%eax)
	movl	%esi, 0x10(%eax)
	movl	%edi, 0x14(%eax)

	leal	8(%esp), %edx
	movl	%edx, 0x18(%eax)
	movl	(%esp), %edx
	movl	%edx, 0x1c(%eax)

	movl	0x8(%esp), %eax
	movl	(%eax), %ebp
	movl	0x04(%eax), %ebx
	movl	0x08(%eax), %ecx
	movl	0x0c(%eax), %edx
	movl	0x10(%eax), %esi
	movl	0x14(%eax), %edi
	movl	0x18(%eax), %esp
	movl	0x1c(%eax), %eax
    pushl   %eax
	xorl	%eax, %eax
	ret


#endif

