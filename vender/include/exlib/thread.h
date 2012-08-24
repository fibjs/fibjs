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

#ifdef _WIN32

class OSMutex
{
public:
	OSMutex()
	{
		InitializeCriticalSection(&cs_);
	}

	~OSMutex()
	{
		DeleteCriticalSection(&cs_);
	}

	int Lock()
	{
		EnterCriticalSection(&cs_);
		return 0;
	}

	int Unlock()
	{
		LeaveCriticalSection(&cs_);
		return 0;
	}

	bool TryLock()
	{
		return !!TryEnterCriticalSection(&cs_);
	}

private:
	CRITICAL_SECTION cs_;
};

#else

class OSMutex
{
public:
	OSMutex()
	{
		pthread_mutexattr_t attrs;
		pthread_mutexattr_init(&attrs);
		pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mutex_, &attrs);
	}

	~OSMutex()
	{
		pthread_mutex_destroy(&mutex_);
	}

	int Lock()
	{
		int result = pthread_mutex_lock(&mutex_);
		return result;
	}

	int Unlock()
	{
		int result = pthread_mutex_unlock(&mutex_);
		return result;
	}

	bool TryLock()
	{
		return !pthread_mutex_trylock(&mutex_);
	}

private:
	pthread_mutex_t mutex_;
};

#endif

class OSThread
{
public:
	OSThread();
	virtual ~OSThread();

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
