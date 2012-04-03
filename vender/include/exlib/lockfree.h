/*
 *  lockfree.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "utils.h"

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


