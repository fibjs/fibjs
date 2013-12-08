#include <osconfig.h>

#if defined(_MSC_VER) && defined(I386)

extern "C" int win_switch();

int __declspec(naked) win_switch()
{
	__asm
	{
		mov eax, [esp + 4]
		mov [eax], ebp
		mov [eax + 004h], ebx
		mov [eax + 008h], ecx
		mov [eax + 00ch], edx
		mov [eax + 010h], esi
		mov [eax + 014h], edi

		lea edx, [esp + 4]
		mov [eax + 018h], edx
		mov edx, [esp]
		mov [eax + 01ch], edx

		mov eax, [esp + 08h]
		mov ebp, [eax]
		mov ebx, [eax + 004h]
		mov ecx, [eax + 008h]
		mov edx, [eax + 00ch]
		mov esi, [eax + 010h]
		mov edi, [eax + 014h]
		mov esp, [eax + 018h]
		mov eax, [eax + 01ch]
		push eax
		xor eax, eax
		ret
	}
}
#endif

#ifndef _WIN32

#ifdef MacOS
asm(".section	__TEXT,__text");
#else
asm(".section	.text");
#endif

#ifdef x64

asm(".globl nix_switch, _nix_switch");
asm("nix_switch:");
asm("_nix_switch:");

asm("   movq	%rcx,0x10(%rdi)");
asm("   movq    %rdi, %rcx");
asm("	movq	%rbp,(%rcx)");
asm("	movq	%rbx,0x08(%rcx)");

asm("   movq	%rdx,0x18(%rcx)");
asm("	movq	%rsi,0x20(%rcx)");
asm("	movq	%rdi,0x28(%rcx)");
asm("	movq	%r12,0x30(%rcx)");
asm("	movq	%r13,0x38(%rcx)");
asm("	movq	%r14,0x40(%rcx)");
asm("	movq	%r15,0x48(%rcx)");

asm("	leaq	8(%rsp),%rax");
asm("	movq	%rax,0x50(%rcx)");

asm("	movq	(%rsp),%rax");
asm("	movq	%rax,0x58(%rcx)");

asm("	movdqa	%xmm6, 0x60(%rcx)");
asm("	movdqa	%xmm7, 0x70(%rcx)");
asm("	movdqa	%xmm8, 0x80(%rcx)");
asm("	movdqa	%xmm9, 0x90(%rcx)");
asm("	movdqa	%xmm10, 0xa0(%rcx)");
asm("	movdqa	%xmm11, 0xb0(%rcx)");
asm("	movdqa	%xmm12, 0xc0(%rcx)");
asm("	movdqa	%xmm13, 0xd0(%rcx)");
asm("	movdqa	%xmm14, 0xe0(%rcx)");
asm("	movdqa	%xmm15, 0xf0(%rcx)");

asm("   movq	%rsi, %rcx");

asm("	movq	(%rcx), %rbp");
asm("	movq	0x08(%rcx), %rbx");

asm("	movq	0x18(%rcx), %rdx");
asm("	movq	0x20(%rcx), %rsi");
asm("	movq	0x28(%rcx), %rdi");
asm("	movq	0x30(%rcx), %r12");
asm("	movq	0x38(%rcx), %r13");
asm("	movq	0x40(%rcx), %r14");
asm("	movq	0x48(%rcx), %r15");
asm("	movq	0x50(%rcx), %rsp");
asm("	movq	0x58(%rcx), %rax");
asm("	movdqa	0x60(%rcx), %xmm6");
asm("	movdqa	0x70(%rcx), %xmm7");
asm("	movdqa	0x80(%rcx), %xmm8");
asm("	movdqa	0x90(%rcx), %xmm9");
asm("	movdqa	0xa0(%rcx), %xmm10");
asm("	movdqa	0xb0(%rcx), %xmm11");
asm("	movdqa	0xc0(%rcx), %xmm12");
asm("	movdqa	0xd0(%rcx), %xmm13");
asm("	movdqa	0xe0(%rcx), %xmm14");
asm("	movdqa	0xf0(%rcx), %xmm15");
asm("	movq	0x10(%rcx), %rcx");
asm("	pushq   %rax");
asm("	xorq	%rax,%rax");
asm("	ret");

#else

asm(".globl nix_switch, _nix_switch");
asm("nix_switch:");
asm("_nix_switch:");

asm("	movl	0x4(%esp), %eax");
asm("	movl	%ebp, (%eax)");
asm("	movl	%ebx, 0x04(%eax)");
asm("	movl	%ecx, 0x08(%eax)");
asm("	movl	%edx, 0x0c(%eax)");
asm("	movl	%esi, 0x10(%eax)");
asm("	movl	%edi, 0x14(%eax)");

asm("	leal	4(%esp), %edx");
asm("	movl	%edx, 0x18(%eax)");
asm("	movl	(%esp), %edx");
asm("	movl	%edx, 0x1c(%eax)");

asm("	movl	0x8(%esp), %eax");
asm("	movl	(%eax), %ebp");
asm("	movl	0x04(%eax), %ebx");
asm("	movl	0x08(%eax), %ecx");
asm("	movl	0x0c(%eax), %edx");
asm("	movl	0x10(%eax), %esi");
asm("	movl	0x14(%eax), %edi");
asm("	movl	0x18(%eax), %esp");
asm("	movl	0x1c(%eax), %eax");
asm("   pushl   %eax");
asm("	xorl	%eax, %eax");
asm("	ret");

#endif

#endif

