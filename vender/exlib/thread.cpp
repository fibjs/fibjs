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

#ifdef Windows

Thread::~Thread()
{
    if(thread_)
        CloseHandle(thread_);
}

void Thread::Start()
{
    DWORD threadid;
    thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, this, 0, &threadid);
}

void Thread::Join()
{
    WaitForSingleObject(thread_, INFINITE);
}

#else

Thread::~Thread()
{
    if(thread_)
        pthread_detach(thread_);
}

void Thread::Start()
{
    pthread_create(&thread_, NULL, ThreadEntry, this);
}

void Thread::Join()
{
    pthread_join(thread_, NULL);
}

#endif

}

