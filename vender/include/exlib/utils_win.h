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

void *_CompareAndSwap(void *volatile *Destination, void *Exchange, void *Comparand);
int32_t CompareAndSwap(int32_t volatile *Destination, int32_t Exchange, int32_t Comparand);

template<typename T>
inline T *CompareAndSwap(T **ptr, T *old_value, T *new_value)
{
    void *result = _CompareAndSwap(
                       reinterpret_cast<void **>(ptr),
                       reinterpret_cast<void *>(new_value), reinterpret_cast<void *>(old_value));
    return reinterpret_cast<T *>(result);
}

int32_t atom_add(int32_t *dest, int32_t incr);
int32_t atom_inc(int32_t *dest);
int32_t atom_dec(int32_t *dest);
int32_t atom_xchg(int32_t *ptr, int32_t new_value);

inline void *CompareAndSwap(void **ptr, void *old_value, void *new_value)
{
    return CompareAndSwap((char **) ptr, (char *) old_value, (char *) new_value);
}

}
