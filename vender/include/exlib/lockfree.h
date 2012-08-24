/*
 *  lockfree.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "utils.h"
#ifndef _WIN32
#include <unistd.h>
#endif

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

    bool empty()
    {
    	return m_first == NULL;
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

        while(CompareAndSwap((void**)&m_lock, (void*)(0), (void*)(-1)));

        do
        {
            p = (T*)m_first;
            if(p == NULL)
                break;
        }while(CompareAndSwap((T**)&m_first, p, (T*)(p->m_next)) != p);

        m_lock = 0;

        if(p != NULL)
            p->m_next = NULL;

        return p;
    }

    T* getList()
    {
        T* p;

        if(m_first == NULL)
            return NULL;

        while(CompareAndSwap((void**)&m_lock, (void*)(0), (void*)(-1)));

        do
        {
            p = (T*)m_first;
            if(p == NULL)
                break;
        }while(CompareAndSwap((T**)&m_first, p, (T*)(NULL)) != p);

        m_lock = 0;

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
            	Sleep(1);
            else if(nCount > 30000)
            	Sleep(0);
        }

        return NULL;
    }

private:
	static void Sleep(int ms)
	{
#ifdef _WIN32
		::Sleep(ms);
#else
		::usleep(1000 * ms);
#endif
	}

private:
    volatile T* m_first;
    volatile void* m_lock;
};

}

#endif


