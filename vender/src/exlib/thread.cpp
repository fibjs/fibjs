/*
 *  thread.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/thread.h>

namespace exlib
{

static void* ThreadEntry(void* arg)
{
    Thread* thread = reinterpret_cast<Thread*>(arg);
    thread->Run();
    return NULL;
}

Thread::Thread() : thread_(0)
{
}

Thread::~Thread()
{
    detach();
}

#ifdef Windows

void Thread::detach()
{
    if(thread_)
        CloseHandle(thread_);
    thread_ = NULL;
}

void Thread::start()
{
    DWORD threadid;
    thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, this, 0, &threadid);
}

void Thread::join()
{
    WaitForSingleObject(thread_, INFINITE);
}

#else

void Thread::detach()
{
    if(thread_)
        pthread_detach(thread_);
    thread_ = (pthread_t)NULL;
}

void Thread::start()
{
    pthread_create(&thread_, NULL, ThreadEntry, this);
}

void Thread::join()
{
    pthread_join(thread_, NULL);
}

#endif

}

