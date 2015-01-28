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
#include "thread.h"

namespace exlib
{

template<typename T>
class lockfree
{
public:
    lockfree() :
        m_first(0), m_lock(0), m_wait_lock(0), m_wait_count(0), m_cond(0)
    {
        m_cond = new OSCondVar(&m_mutex);
    }

    ~lockfree(){
        delete m_cond;
        m_cond = 0;
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

        while (CompareAndSwap(&m_wait_lock, 0, -1));

        bool has_wait = (m_wait_count > 0);

        atom_xchg(&m_wait_lock, 0);

        if (has_wait){
            m_mutex.Lock();
            m_cond->Signal();
            m_mutex.Unlock();
        }
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
        T *p;
        // sleep first let other thread set data
        #ifdef _WIN32
        ::Sleep(0);
        #else
        ::usleep(0);
        #endif
        p = get();
        if (p == 0){
            // first lock the mutex
            m_mutex.Lock();

            // second lock the lockfree var
            while (CompareAndSwap(&m_wait_lock, 0, -1));
            m_wait_count++;
            atom_xchg(&m_wait_lock, 0);

            while (p == 0){
                // wait for the signal
                m_cond->Wait();
                // get item
                p = get();
            }

            // second lock the lockfree var
            while (CompareAndSwap(&m_wait_lock, 0, -1));
            m_wait_count--;
            atom_xchg(&m_wait_lock, 0);

            // unlock the mutex
            m_mutex.Unlock();
        }
        return p;
    }

private:
    T *volatile  m_first;
    volatile int32_t m_lock;
    volatile int32_t m_wait_lock;
    volatile uint32_t m_wait_count;
    OSMutex m_mutex;
    OSCondVar *m_cond;
};

}

#endif

