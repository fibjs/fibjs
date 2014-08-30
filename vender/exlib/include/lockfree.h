/*
 *  lockfree.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#ifndef _ex_lockfree_h__
#define _ex_lockfree_h__

#ifndef _WIN32
#include <unistd.h>
#endif

#include "utils.h"

namespace exlib
{

template<typename T>
class lockfree
{
public:
    lockfree() :
        m_first(0), m_lock(0)
    {
    }

    bool empty()
    {
        return m_first == 0;
    }

    void put(T *o)
    {
        while (CompareAndSwap(&m_lock, 0, -1));

        o->m_next = m_first;
        m_first = o;

        atom_xchg(&m_lock, 0);
    }

    T *get()
    {
        T *p;

        while (CompareAndSwap(&m_lock, 0, -1));

        p = m_first;
        if (p)
        {
            m_first = (T *)p->m_next;
            p->m_next = 0;
        }

        atom_xchg(&m_lock, 0);
        return p;
    }

    T *getList()
    {
        T *p;

        while (CompareAndSwap(&m_lock, 0, -1));

        p = m_first;
        m_first = 0;

        atom_xchg(&m_lock, 0);
        return p;
    }

    T *wait()
    {
        int nCount = 0;
        int time_0 = 0;
        int time_1 = time_0 + 2000;
        int time_2 = time_1 + 200;
        int time_3 = time_2 + 20;

        while (1)
        {
            if (nCount < 2000000)
                nCount++;

            T *p = get();
            if (p != 0)
                return p;

            if (nCount > time_3)
                Sleep(100);
            else if (nCount > time_2)
                Sleep(10);
            else if (nCount > time_1)
                Sleep(1);
            else if (nCount > time_0)
                Sleep(0);
        }

        return 0;
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
    T *volatile  m_first;
    volatile int32_t m_lock;
};

}

#endif

