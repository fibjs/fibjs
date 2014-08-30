/*
 *  utils_win.h
 *  Created on: Jul 17, 2014
 *
 *  Copyright (c) 2014 by Leo Hoo
 *  lion@9465.net
 */

#include <windows.h>

namespace exlib
{

#ifdef x64
inline void MemoryBarrier()
{
    ::__faststorefence();
}
#else
inline void MemoryBarrier()
{
    LONG Barrier;
    __asm
    {
        xchg Barrier, eax
    }
}
#endif

int32_t CompareAndSwap(volatile int32_t *ptr, int32_t old_value, int32_t new_value);

void *_CompareAndSwap(void *volatile *Destination, void *Exchange, void *Comparand);

template<typename T>
inline T *CompareAndSwap(T *volatile *ptr, T *old_value, T *new_value)
{
    void *result = _CompareAndSwap((void *volatile *)ptr, (void *)new_value, (void *)old_value);
    return (T *)result;
}

inline void *CompareAndSwap(void *volatile *ptr, void *old_value, void *new_value)
{
    return _CompareAndSwap(ptr, old_value, new_value);
}

void *_atom_xchg(void *volatile *ptr, void *new_value);

template<typename T>
inline T *atom_xchg(T *volatile *ptr, T *new_value)
{
    void *result = _atom_xchg((void *volatile *)ptr, (void *)new_value);
    return (T *)result;
}

inline void *atom_xchg(void *volatile *ptr, void *new_value)
{
    return _atom_xchg(ptr, new_value);
}

int32_t atom_add(volatile int32_t *dest, int32_t incr);
int32_t atom_inc(volatile int32_t *dest);
int32_t atom_dec(volatile int32_t *dest);
int32_t atom_xchg(volatile int32_t *ptr, int32_t new_value);

}
