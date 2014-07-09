/*
 *  utils_x86.h
 *  Created on: Jul 17, 2014
 *
 *  Copyright (c) 2014 by Leo Hoo
 *  lion@9465.net
 */

#ifdef MacOS
#include <libkern/OSAtomic.h>
#endif

namespace exlib
{

#if defined(MacOS)
inline void MemoryBarrier()
{
    OSMemoryBarrier();
}
#else
inline void MemoryBarrier()
{
    __asm__ __volatile__("" : : : "memory");
}
#endif

template<typename T>
inline T *CompareAndSwap(T **ptr, T *old_value, T *new_value)
{
    T *prev;
    __asm__ __volatile__(
        "lock; cmpxchgl %1,%2"
        : "=a" (prev)
        : "q" (new_value), "m" (*ptr), "0" (old_value)
        : "memory");
    return prev;
}

inline int32_t CompareAndSwap(int32_t *ptr, int32_t old_value, int32_t new_value)
{
    int32_t prev;
    __asm__ __volatile__(
        "lock; cmpxchgl %1,%2"
        : "=a" (prev)
        : "q" (new_value), "m" (*ptr), "0" (old_value)
        : "memory");
    return prev;
}

inline int32_t atom_add(__volatile__ int32_t *dest, int32_t incr)
{
    int32_t ret;
    __asm__ __volatile__(
        "lock; xaddl %0,(%1)"
        : "=r"(ret)
        : "r"(dest), "0"(incr)
        : "memory");
    return ret + incr;
}

inline int32_t atom_xchg(int32_t *ptr, int32_t new_value)
{
    int32_t prev;

    __asm__ __volatile__(
        "lock xchgl %2,(%1)"
        : "=r" (prev)
        : "r" (ptr), "0" (new_value)
        : "memory");

    return prev;
}

inline int32_t atom_inc(__volatile__ int32_t *dest)
{
    return atom_add(dest, 1);
}

inline int32_t atom_dec(__volatile__ int32_t *dest)
{
    return atom_add(dest, -1);
}

inline void *CompareAndSwap(void **ptr, void *old_value, void *new_value)
{
    return CompareAndSwap((char **) ptr, (char *) old_value, (char *) new_value);
}

}
