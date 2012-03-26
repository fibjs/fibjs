#include <osconfig.h>

#if defined(_MSC_VER) && defined(I386)

extern "C" int win_switch();

int __declspec(naked) win_switch()
{
	__asm{
		mov		eax, [esp + 4]
		mov		[eax], ebp
		mov		[eax + 004h], ebx
		mov		[eax + 008h], ecx
		mov		[eax + 00ch], edx
		mov		[eax + 010h], esi
		mov		[eax + 014h], edi

		lea		edx, [esp + 4]
		mov		[eax + 018h], edx
		mov		edx, [esp]
		mov		[eax + 01ch], edx

		mov		eax, [esp + 08h]
		mov		ebp, [eax]
		mov		ebx, [eax + 004h]
		mov		ecx, [eax + 008h]
		mov		edx, [eax + 00ch]
		mov		esi, [eax + 010h]
		mov		edi, [eax + 014h]
		mov		esp, [eax + 018h]
		mov		eax, [eax + 01ch]
		push	eax
		xor		eax, eax
		ret
	}
}
#endif
