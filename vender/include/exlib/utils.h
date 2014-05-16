/*
 *  utils.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "../osconfig.h"

#ifndef _ex_utils_h__
#define _ex_utils_h__

namespace exlib
{

#ifdef WIN32

void *_CompareAndSwap(
    void *volatile *Destination,
    void *Exchange,
    void *Comparand
);

template<typename T>
inline T *CompareAndSwap(T **ptr, T *old_value, T *new_value)
{
    void *result = _CompareAndSwap(
                       reinterpret_cast<void **>(ptr),
                       reinterpret_cast<void *>(new_value), reinterpret_cast<void *>(old_value));
    return reinterpret_cast<T *>(result);
}

int atom_add(int *dest, int incr);
int atom_inc(int *dest);
int atom_dec(int *dest);
int atom_xchg(int *ptr, int new_value);

#else

#if defined(arm)

template<typename T>
inline T *CompareAndSwap(T **ptr, T *old_value, T *new_value)
{
    T *oldval, *res;
    do
    {
        __asm__ __volatile__(
            "ldrex %0, [%3]\n"
            "mov %1, #0\n"
            "cmp %0, %4\n"
#ifdef __thumb2__
            "it eq\n"
#endif
            "strexeq %1, %5, [%3]\n"
            : "=&r"(oldval), "=&r"(res), "+m"(*ptr)
            : "r"(ptr), "r"(old_value), "r"(new_value)
            : "cc", "memory");
    }
    while (res != 0);
    return oldval;

}

inline int atom_add(__volatile__ int *dest, int incr)
{
    unsigned long value;
    int res;

    do
    {
        __asm__ __volatile__(
            "ldrex %0, [%3]\n"
            "add %0, %0, %4\n"
            "strex %1, %0, [%3]\n"
            : "=&r"(value), "=&r"(res), "+m"(*dest)
            : "r"(dest), "r"(incr)
            : "cc", "memory");
    }
    while (res);
    return value;
}

inline int atom_xchg(int *ptr, int new_value)
{
    int old_value;
    int res;

    do
    {
        __asm__ __volatile__(
            "ldrex %0, [%3]\n"
            "strex %1, %4, [%3]\n"
            : "=&r"(old_value), "=&r"(res), "+m"(*ptr)
            : "r"(ptr), "r"(new_value)
            : "cc", "memory");
    }
    while (res != 0);
    return old_value;
}

#else

#if defined(x64)

template<typename T>
inline T *CompareAndSwap(T **ptr, T *old_value, T *new_value)
{
    T *prev;
    __asm__ __volatile__(
        "lock; cmpxchgq %1,%2"
        : "=a" (prev)
        : "q" (new_value), "m" (*ptr), "0" (old_value)
        : "memory");
    return prev;
}

#else

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

#endif

inline int atom_add(__volatile__ int *dest, int incr)
{
    int ret;
    __asm__ __volatile__(
        "lock; xaddl %0,(%1)"
        : "=r"(ret)
        : "r"(dest), "0"(incr)
        : "memory");
    return ret + incr;
}

inline int atom_xchg(int *ptr, int new_value)
{
    int prev;

    __asm__ __volatile__(
        "lock xchgl %2,(%1)"
        : "=r" (prev)
        : "r" (ptr), "0" (new_value)
        : "memory");

    return prev;
}

#endif

inline int atom_inc(__volatile__ int *dest)
{
    return atom_add(dest, 1);
}

inline int atom_dec(__volatile__ int *dest)
{
    return atom_add(dest, -1);
}

#endif

inline void *CompareAndSwap(void **ptr, void *old_value, void *new_value)
{
    return CompareAndSwap((char **) ptr, (char *) old_value, (char *) new_value);
}

}

#endif
