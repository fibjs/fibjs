/*
 *  thread.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "../osconfig.h"

#ifdef Windows
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#ifndef _ex_thread_h__
#define _ex_thread_h__

namespace exlib
{

class Thread
{
public:
    Thread();
    virtual ~Thread();

    void start();
    void detach();
    void join();
    virtual void Run() = 0;

    static void Sleep(int ms)
    {
#ifdef Windows
        ::Sleep(ms);
#else
        ::usleep(1000 * ms);
#endif
    }

private:
#ifdef Windows
    HANDLE thread_;
#else
    pthread_t thread_;
#endif
};

}

#endif
