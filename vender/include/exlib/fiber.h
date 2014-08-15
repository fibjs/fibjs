/*
 *  fiber.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <osconfig.h>
#include "event.h"
#include "lockfree.h"

#ifndef _ex_fiber_h__
#define _ex_fiber_h__

namespace exlib
{

#pragma pack (1)

#if defined(x64)

typedef unsigned long long reg_type;

typedef struct
{
    reg_type Rbp;
    reg_type Rbx;
    reg_type Rcx;
    reg_type Rdx;
    reg_type Rsi;
    reg_type Rdi;
    reg_type R12;
    reg_type R13;
    reg_type R14;
    reg_type R15;
    reg_type Rsp;
    reg_type Rip;
} context;

#elif defined(I386)

typedef unsigned long reg_type;

typedef struct __JUMP_BUFFER
{
    reg_type Ebp;
    reg_type Ebx;
    reg_type Ecx;
    reg_type Edx;
    reg_type Esi;
    reg_type Edi;
    reg_type Esp;
    reg_type Eip;
} context;

#elif defined(arm)

typedef unsigned long reg_type;

typedef struct __JUMP_BUFFER
{
    reg_type r0;
    reg_type r1;
    reg_type r2;
    reg_type r3;
    reg_type r4;
    reg_type r5;
    reg_type r6;
    reg_type r7;
    reg_type r8;
    reg_type r9;
    reg_type r10;
    reg_type r11;
    reg_type r12;
    reg_type r13;
    reg_type r14;
} context;

#endif
#pragma pack ()

#define TLS_SIZE    8

#define FIBER_STACK_SIZE    (65536 * 2)

class AsyncEvent : public linkitem
{
public:
    virtual ~AsyncEvent()
    {}

    virtual int post(int v);
    virtual int apost(int v)
    {
        return post(v);
    }

    virtual void callback()
    {
        weak.set();
    }

    int wait()
    {
        weak.wait();
        return m_v;
    }

    bool isSet()
    {
        return weak.isSet();
    }

    int result()
    {
        return m_v;
    }

    void sleep(int ms);

private:
    Event weak;
    int m_v;
};

class Fiber : public linkitem
{
public:
    void Ref()
    {
        refs_++;
    }

    void Unref()
    {
        if (--refs_ == 0)
            destroy();
    }

    void join()
    {
        m_joins.wait();
    }

    static void sleep(int ms);
    static void yield();
    static Fiber *Current();

private:
    void destroy();

public:
    reg_type refs_;
    context m_cntxt;
    Event m_joins;
    void *m_tls[TLS_SIZE];
};

typedef void (*IDLE_PROC)();

class Service
{
public:
    Service();

public:
    static Fiber *CreateFiber(void *(*func)(void *), void *data = 0,
                              int stacksize = FIBER_STACK_SIZE);

    static int tlsAlloc();
    static void *tlsGet(int idx);
    static void tlsPut(int idx, void *v);
    static void tlsFree(int idx);

public:
    void switchtonext();
    void yield();
    static Service *getFiberService();
    static bool hasService();

    IDLE_PROC onIdle(IDLE_PROC proc)
    {
        IDLE_PROC p = m_Idle;
        m_Idle = proc;
        return p;
    }

public:
    Fiber m_main;
    Fiber *m_running;
    Fiber *m_recycle;
    char m_tls[TLS_SIZE];
    List<Fiber> m_resume;
    lockfree<AsyncEvent> m_aEvents;
    List<AsyncEvent> m_yieldList;

    IDLE_PROC m_Idle;

    static Service *root;
};

typedef lockfree<AsyncEvent> AsyncQueue;
}

#endif

