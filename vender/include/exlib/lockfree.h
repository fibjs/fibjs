/*
 *  lockfree.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "../osconfig.h"

#ifdef _MSC_VER
#include <winbase.h>
#endif

#ifndef lockfree_h__
#define lockfree_h__

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

template<typename T>
class node_stub
{
public:
    node_stub(T v) : value(v)
    {
    }

    node_stub()
    {
    }

public:
    T value;
    node_stub* m_next;
};

template<typename T>
class lockfree
{
public:
    lockfree() : m_first(NULL), m_lock(NULL)
    {
    }

    void put(T* o)
    {
        T* p;

        do
        {
            p = m_first;
            o->m_next = p;
        }while(CompareAndSwap(&m_first, p, o) != p);
    }

    T* get()
    {
        T* p;

        while(CompareAndSwap(&m_lock, reinterpret_cast<T*>(0), reinterpret_cast<T*>(-1)));

        do
        {
            p = m_first;
            if(p == NULL)
                break;
        }while(CompareAndSwap(&m_first, p, p->m_next) != p);

        m_lock = NULL;

        if(p != NULL)
            p->m_next = NULL;

        return p;
    }

private:
    T* m_first;
    T* m_lock;
};

}

#endif


