/*
 *  event.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <osconfig.h>
#include "lockfree.h"

#ifndef _ex_event_h__
#define _ex_event_h__

namespace exlib
{

class linkitem
{
public:
    linkitem() : m_next(0)
    {
    }

public:
    linkitem *m_next;
};

template<class T>
class List
{
public:
    List() :
        m_first(0), m_last(0), m_count(0)
    {
    }

    void put(T *pNew)
    {
        if (m_last)
            m_last->m_next = pNew;
        else
            m_first = pNew;

        m_last = pNew;

        m_count++;
    }

    T *get()
    {
        T *pNow = (T *)m_first;

        if (pNow)
        {
            m_first = (T *)pNow->m_next;
            if (m_first == 0)
                m_last = 0;

            pNow->m_next = 0;

            m_count--;
        }

        return pNow;
    }

    bool empty() const
    {
        return m_count == 0;
    }

    int count() const
    {
        return m_count;
    }

private:
    volatile T *m_first;
    volatile T *m_last;
    int m_count;
};

class Locker
{
public:
    Locker() :
        m_locked(false), m_count(0), m_locker(0)
    {
    }

public:
    void lock();
    void unlock();
    bool trylock();
    bool owned();

    int blocked() const
    {
        return m_blocks.count();
    }

private:
    bool m_locked;
    int m_count;
    linkitem *m_locker;
    List<linkitem> m_blocks;
};

class autoLocker
{
public:
    autoLocker(Locker &l) :
        m_l(l)
    {
        m_l.lock();
    }

    ~autoLocker()
    {
        m_l.unlock();
    }

private:
    Locker &m_l;
};

class Event
{
public:
    Event()
    {
        m_set = false;
    }

public:
    void wait();
    void pulse();
    void set();
    void reset();
    bool isSet();

private:
    bool m_set;
    List<linkitem> m_blocks;
};

class CondVar
{
public:
    void wait(Locker &l);
    void notify_one();
    void notify_all();

    int blocked() const
    {
        return m_blocks.count();
    }

private:
    List<linkitem> m_blocks;
};

class Semaphore
{
public:
    Semaphore(int count = 0) :
        m_count(count)
    {
    }

public:
    void wait();
    void post();
    bool trywait();

    int blocked() const
    {
        return m_blocks.count();
    }

private:
    int m_count;
    List<linkitem> m_blocks;
};

template<class T>
class Queue
{
public:
    void put(T *pNew)
    {
        m_list.put(pNew);
        m_sem.post();
    }

    T *get()
    {
        m_sem.wait();
        return m_list.get();
    }

    T *tryget()
    {
        if (!m_sem.trywait())
            return NULL;
        return m_list.get();
    }

    bool empty() const
    {
        return m_list.empty();
    }

    int count() const
    {
        return m_list.count();
    }

public:
    List<T> m_list;
    Semaphore m_sem;
};

}

#endif

