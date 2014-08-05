/*
 * fbUtils.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: lion
 */

#ifdef WIN32

#include <exlib/utils.h>
#include <windows.h>
#include <stdio.h>

namespace exlib
{

void *_CompareAndSwap(void *volatile *Destination, void *Exchange, void *Comparand)
{
    return InterlockedCompareExchangePointer(Destination, Exchange, Comparand);
}

int32_t CompareAndSwap(int32_t volatile *Destination, int32_t old_value, int32_t new_value)
{
    return InterlockedCompareExchange((LONG *)Destination, new_value, old_value);
}

int32_t atom_add(int32_t *dest, int32_t incr)
{
    return InterlockedExchangeAdd((LONG *)dest, incr) + incr;
}

int32_t atom_inc(int32_t *dest)
{
    return InterlockedIncrement((LONG *)dest);
}

int32_t atom_dec(int32_t *dest)
{
    return InterlockedDecrement((LONG *)dest);
}

int32_t atom_xchg(int32_t *ptr, int32_t new_value)
{
    return InterlockedExchange((LONG *)ptr, new_value);
}

}

#endif
