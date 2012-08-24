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
	OSThread* thread = reinterpret_cast<OSThread*>(arg);
    thread->Run();
    return NULL;
}

OSThread::OSThread() : thread_(0)
{
}

OSThread::~OSThread()
{
    detach();
}

#ifdef _WIN32

OSSemaphore::OSSemaphore(int start_val)
{
	m_handle = ::CreateSemaphore(NULL, start_val, 1, NULL);
}

void OSThread::detach()
{
    if(thread_)
        CloseHandle(thread_);
    thread_ = NULL;
}

void OSThread::start()
{
    DWORD threadid;
    thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, this, 0, &threadid);
}

void OSThread::join()
{
    WaitForSingleObject(thread_, INFINITE);
}

#else

void OSThread::detach()
{
    if(thread_)
        pthread_detach(thread_);
    thread_ = (pthread_t)NULL;
}

void OSThread::start()
{
    pthread_create(&thread_, NULL, ThreadEntry, this);
}

void OSThread::join()
{
    pthread_join(thread_, NULL);
}

#endif

}

