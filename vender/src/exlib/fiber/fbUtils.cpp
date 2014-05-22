/*
 * fbUtils.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: lion
 */

#ifdef WIN32

#include <exlib/utils.h>
#include <windows.h>

namespace exlib
{

void* _CompareAndSwap(
		void* volatile *Destination,
		void* Exchange,
		void* Comparand
)
{
	return InterlockedCompareExchangePointer(Destination, Exchange, Comparand);
}

int atom_add(int *dest, int incr)
{
	return InterlockedExchangeAdd((LONG*)dest, incr) + incr;
}

int atom_inc(int *dest)
{
	return InterlockedIncrement((LONG*)dest);
}

int atom_dec(int *dest)
{
	return InterlockedDecrement((LONG*)dest);
}

int atom_xchg(int* ptr, int new_value)
{
	return InterlockedExchange((LONG*)ptr, new_value);
}

}

#endif
