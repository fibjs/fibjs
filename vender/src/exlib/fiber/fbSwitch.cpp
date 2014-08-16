#include "osconfig.h"

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
asm(".section   __TEXT,__text");
#else
asm(".section   .text");
#endif

asm(".globl nix_switch, _nix_switch");
asm("nix_switch:");
asm("_nix_switch:");

#if defined(x64)

asm("    movq    %rbp,(%rdi)");

asm("    movq    %rbx,0x08(%rdi)");
asm("    movq    %rcx,0x10(%rdi)");
asm("    movq    %rdx,0x18(%rdi)");
asm("    movq    %rsi,0x20(%rdi)");
asm("    movq    %rdi,0x28(%rdi)");
asm("    movq    %r12,0x30(%rdi)");
asm("    movq    %r13,0x38(%rdi)");
asm("    movq    %r14,0x40(%rdi)");
asm("    movq    %r15,0x48(%rdi)");

asm("    leaq    8(%rsp),%rax");
asm("    movq    %rax,0x50(%rdi)");

asm("    movq    (%rsp),%rax");
asm("    movq    %rax,0x58(%rdi)");

asm("    movq    %rsi, %rdi");

asm("    movq    (%rdi), %rbp");
asm("    movq    0x08(%rdi), %rbx");
asm("    movq    0x10(%rdi), %rcx");
asm("    movq    0x18(%rdi), %rdx");
asm("    movq    0x20(%rdi), %rsi");

asm("    movq    0x30(%rdi), %r12");
asm("    movq    0x38(%rdi), %r13");
asm("    movq    0x40(%rdi), %r14");
asm("    movq    0x48(%rdi), %r15");
asm("    movq    0x50(%rdi), %rsp");

asm("    movq    0x58(%rdi), %rax");

asm("    movq    0x28(%rdi), %rdi");

asm("    pushq   %rax");
asm("    xorq    %rax,%rax");
asm("    ret");

#elif defined(I386)

asm("    movl    0x4(%esp), %eax");
asm("    movl    %ebp, (%eax)");
asm("    movl    %ebx, 0x04(%eax)");
asm("    movl    %ecx, 0x08(%eax)");
asm("    movl    %edx, 0x0c(%eax)");
asm("    movl    %esi, 0x10(%eax)");
asm("    movl    %edi, 0x14(%eax)");

asm("    leal    4(%esp), %edx");
asm("    movl    %edx, 0x18(%eax)");
asm("    movl    (%esp), %edx");
asm("    movl    %edx, 0x1c(%eax)");

asm("    movl    0x8(%esp), %eax");
asm("    movl    (%eax), %ebp");
asm("    movl    0x04(%eax), %ebx");
asm("    movl    0x08(%eax), %ecx");
asm("    movl    0x0c(%eax), %edx");
asm("    movl    0x10(%eax), %esi");
asm("    movl    0x14(%eax), %edi");
asm("    movl    0x18(%eax), %esp");
asm("    movl    0x1c(%eax), %eax");
asm("    pushl   %eax");
asm("    xorl    %eax, %eax");
asm("    ret");

#elif defined(arm)

asm("    str r0, [r0]");
asm("    str r1, [r0,#4]");
asm("    str r2, [r0,#8]");
asm("    str r3, [r0,#12]");
asm("    str r4, [r0,#16]");
asm("    str r5, [r0,#20]");
asm("    str r6, [r0,#24]");
asm("    str r7, [r0,#28]");
asm("    str r8, [r0,#32]");
asm("    str r9, [r0,#36]");
asm("    str r10, [r0,#40]");
asm("    str r11, [r0,#44]");
asm("    str r12, [r0,#48]");
asm("    str r13, [r0,#52]");
asm("    str r14, [r0,#56]");

asm("    mov r0, r1");

asm("    ldr r1, [r0,#4]");
asm("    ldr r2, [r0,#8]");
asm("    ldr r3, [r0,#12]");
asm("    ldr r4, [r0,#16]");
asm("    ldr r5, [r0,#20]");
asm("    ldr r6, [r0,#24]");
asm("    ldr r7, [r0,#28]");
asm("    ldr r8, [r0,#32]");
asm("    ldr r9, [r0,#36]");
asm("    ldr r10, [r0,#40]");
asm("    ldr r11, [r0,#44]");
asm("    ldr r12, [r0,#48]");
asm("    ldr r13, [r0,#52]");
asm("    ldr r14, [r0,#56]");
asm("    ldr r0, [r0]");
asm("    mov pc, lr");

#endif

#endif

