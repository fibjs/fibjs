/*
 *  pcl_setjmp.h
 *  Created on: Mar 4, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#ifndef PCL_SETJMP_H_
#define PCL_SETJMP_H_

#pragma pack (1)
#if defined(__x86_64)

typedef struct {
	unsigned long long Part[2];
}SETJMP_FLOAT128;

typedef struct {
	unsigned long long Rbp;
	unsigned long long Rbx;
	unsigned long long Rsi;
	unsigned long long Rdi;
	unsigned long long R12;
	unsigned long long R13;
	unsigned long long R14;
	unsigned long long R15;
	unsigned long long Rsp;
	unsigned long long Rip;
	SETJMP_FLOAT128 Xmm6;
	SETJMP_FLOAT128 Xmm7;
	SETJMP_FLOAT128 Xmm8;
	SETJMP_FLOAT128 Xmm9;
	SETJMP_FLOAT128 Xmm10;
	SETJMP_FLOAT128 Xmm11;
	SETJMP_FLOAT128 Xmm12;
	SETJMP_FLOAT128 Xmm13;
	SETJMP_FLOAT128 Xmm14;
	SETJMP_FLOAT128 Xmm15;
} pcl_buf;

#else

typedef struct __JUMP_BUFFER {
	unsigned long Ebp;
	unsigned long Ebx;
	unsigned long Ecx;
	unsigned long Esi;
	unsigned long Edi;
	unsigned long Esp;
	unsigned long Eip;
} pcl_buf;

#endif
#pragma pack ()

#if defined(Windows)

extern int win_setjmp(pcl_buf* _Buf);
extern int win_longjmp(pcl_buf* _Buf, int _Value);

#define pcl_setjmp win_setjmp
#define pcl_longjmp win_longjmp

#else

extern int nix_setjmp(pcl_buf* _Buf);
extern int nix_longjmp(pcl_buf* _Buf, int _Value);

#define pcl_setjmp nix_setjmp
#define pcl_longjmp nix_longjmp

#endif

#endif /* PCL_SETJMP_H_ */
