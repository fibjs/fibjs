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
        T *p;

        do
        {
            p = (T *) m_first;
            o->m_next = p;
        }
        while (CompareAndSwap((T **) &m_first, p, o) != p);
    }

    T *get()
    {
        T *p;

        if (m_first == 0)
            return 0;

        while (CompareAndSwap((void **) &m_lock, (void *) (0), (void *) (-1)))
            ;

        do
        {
            p = (T *) m_first;
            if (p == 0)
                break;
        }
        while (CompareAndSwap((T **) &m_first, p, (T *) (p->m_next)) != p);

        m_lock = 0;

        if (p != 0)
            p->m_next = 0;

        return p;
    }

    T *getList()
    {
        T *p;

        if (m_first == 0)
            return 0;

        while (CompareAndSwap((void **) &m_lock, (void *) (0), (void *) (-1)))
            ;

        do
        {
            p = (T *) m_first;
            if (p == 0)
                break;
        }
        while (CompareAndSwap((T **) &m_first, p, (T *) (0)) != p);

        m_lock = 0;

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
    volatile T *m_first;
    volatile void *m_lock;
};

}

#endif

