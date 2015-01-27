/*
 *  fiber.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#ifndef _ex_fiber_h__
#define _ex_fiber_h__

#include "osconfig.h"
#include "event.h"

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
        wake.set();
    }

    int wait()
    {
        wake.wait();
        return m_v;
    }

    bool isSet()
    {
        return wake.isSet();
    }

    int result()
    {
        return m_v;
    }

    void sleep(int ms);

private:
    Event wake;
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

public:
    static void sleep(int ms);
    static void yield();
    static Fiber *Current();
    static Fiber *Create(void *(*func)(void *), void *data = 0,
                         int stacksize = FIBER_STACK_SIZE);

    static int tlsAlloc();
    static void *tlsGet(int idx);
    static void tlsPut(int idx, void *v);
    static void tlsFree(int idx);

private:
    void destroy();

public:
    reg_type refs_;
    context m_cntxt;
    Event m_joins;
    void *m_tls[TLS_SIZE];
};

}

#endif

