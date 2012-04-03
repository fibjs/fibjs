/*
 *  utils.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "../osconfig.h"

#ifdef _MSC_VER
#include <winbase.h>
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

#else

#ifdef x64

template<typename T>
inline T* CompareAndSwap(T** ptr,  T* old_value, T* new_value)
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

#endif

}

#endif
