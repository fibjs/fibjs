/*
 *  utils.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "../osconfig.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

#ifndef _ex_utils_h__
#define _ex_utils_h__

namespace exlib
{

#ifdef _MSC_VER

template<typename T>
inline T* CompareAndSwap(T** ptr, T* old_value, T* new_value)
{
	PVOID result = InterlockedCompareExchangePointer(
			reinterpret_cast<PVOID*>(ptr),
			reinterpret_cast<PVOID>(new_value), reinterpret_cast<PVOID>(old_value));
	return reinterpret_cast<T*>(result);
}

inline int atom_add(int *dest, int incr)
{
	return InterlockedAdd((LONG*)dest, incr);
}

inline int atom_inc(int *dest)
{
	return InterlockedIncrement((LONG*)dest);
}

inline int atom_dec(int *dest)
{
	return InterlockedDecrement((LONG*)dest);
}

#else

#ifdef x64

template<typename T>
inline T* CompareAndSwap(T** ptr, T* old_value, T* new_value)
{
	T* prev;
	__asm__ __volatile__("lock; cmpxchgq %1,%2"
			: "=a" (prev)
			: "q" (new_value), "m" (*ptr), "0" (old_value)
			: "memory");
	return prev;
}

#else

template<typename T>
inline T* CompareAndSwap(T** ptr, T* old_value, T* new_value)
{
	T* prev;
	__asm__ __volatile__("lock; cmpxchgl %1,%2"
			: "=a" (prev)
			: "q" (new_value), "m" (*ptr), "0" (old_value)
			: "memory");
	return prev;
}

#endif

inline int atom_add(__volatile__ int *dest, int incr)
{
	int ret;
	__asm__ __volatile__("lock; xaddl %0,(%1)"
			: "=r"(ret)
			: "r"(dest), "0"(incr)
			: "memory");
	return ret + incr;
}

inline int atom_inc(__volatile__ int *dest)
{
	return atom_add(dest, 1);
}

inline int atom_dec(__volatile__ int *dest)
{
	return atom_add(dest, -1);
}

#endif

}

#endif
