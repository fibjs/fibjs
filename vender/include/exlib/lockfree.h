/*
 *  lockfree.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "utils.h"
#include "thread.h"

#ifndef _ex_lockfree_h__
#define _ex_lockfree_h__

namespace exlib
{

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
            p = (T*)m_first;
            o->m_next = p;
        }while(CompareAndSwap((T**)&m_first, p, o) != p);
    }

    T* get()
    {
        T* p;

        if(m_first == NULL)
            return NULL;

        while(CompareAndSwap(&m_lock, reinterpret_cast<T*>(0), reinterpret_cast<T*>(-1)));

        do
        {
            p = (T*)m_first;
            if(p == NULL)
                break;
        }while(CompareAndSwap((T**)&m_first, p, reinterpret_cast<T*>(p->m_next)) != p);

        m_lock = 0;

        if(p != NULL)
            p->m_next = NULL;

        return p;
    }

    T* wait()
    {
        int nCount = 0;

        while(1)
        {
            if(nCount < 60000)
                nCount ++;

            T* p = get();
            if(p != NULL)
                return p;

            if(nCount > 55000)
                Thread::Sleep(1);
            else if(nCount > 30000)
                Thread::Sleep(0);
        }

        return NULL;
    }

private:
    volatile T* m_first;
    T* m_lock;
};

}

#endif


