/*
 *  utils_arm.h
 *  Created on: Jul 10, 2014
 *
 *  Copyright (c) 2014 by Leo Hoo
 *  lion@9465.net
 */

namespace exlib
{

typedef void (*LinuxKernelMemoryBarrierFunc)(void);
inline void MemoryBarrier()
{
    (*(LinuxKernelMemoryBarrierFunc)0xffff0fa0)();
}

template<typename T>
inline T *CompareAndSwap(T *volatile *ptr, T *old_value, T *new_value)
{
    T *oldval, *res;

    do
    {
        __asm__ volatile(
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

inline int32_t CompareAndSwap(volatile int32_t *ptr, int32_t old_value, int32_t new_value)
{
    int32_t oldval, res;

    do
    {
        __asm__ volatile(
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

inline int32_t atom_add(volatile int32_t *dest, int32_t incr)
{
    int32_t value;
    int32_t res;

    do
    {
        __asm__ volatile(
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

inline int32_t atom_inc(volatile int32_t *dest)
{
    return atom_add(dest, 1);
}

inline int32_t atom_dec(volatile int32_t *dest)
{
    return atom_add(dest, -1);
}

inline int32_t atom_xchg(volatile int32_t *ptr, int32_t new_value)
{
    int32_t old_value;
    int32_t res;

    do
    {
        __asm__ volatile(
            "ldrex %0, [%3]\n"
            "strex %1, %4, [%3]\n"
            : "=&r"(old_value), "=&r"(res), "+m"(*ptr)
            : "r"(ptr), "r"(new_value)
            : "cc", "memory");
    }
    while (res != 0);
    return old_value;
}

template<typename T>
inline T *atom_xchg(T *volatile *ptr, T *new_value)
{
    T *old_value;
    T *res;

    do
    {
        __asm__ volatile(
            "ldrex %0, [%3]\n"
            "strex %1, %4, [%3]\n"
            : "=&r"(old_value), "=&r"(res), "+m"(*ptr)
            : "r"(ptr), "r"(new_value)
            : "cc", "memory");
    }
    while (res != 0);
    return old_value;
}

inline void *CompareAndSwap(void *volatile *ptr, void *old_value, void *new_value)
{
    return CompareAndSwap((char *volatile *) ptr, (char *) old_value, (char *) new_value);
}

}
