/*
 *  fiber.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <string.h>
#include <osconfig.h>
#include <stdio.h>
#include <stdlib.h>

#include "lockfree.h"

#ifndef _ex_fiber_h__
#define _ex_fiber_h__

namespace exlib
{

#pragma pack (1)
#if defined(x64)

typedef struct
{
	unsigned long long Part[2];
} SETJMP_FLOAT128;

typedef struct
{
	unsigned long long Rbp;
	unsigned long long Rbx;
	unsigned long long Rcx;
	unsigned long long Rdx;
	unsigned long long Rsi;
	unsigned long long Rdi;
	unsigned long long R12;
	unsigned long long R13;
	unsigned long long R14;
	unsigned long long R15;
	unsigned long long Rsp;
	unsigned long long Rip;
	SETJMP_FLOAT128 Xmm6;
	SETJMP_FLOAT128 Xmm7;
	SETJMP_FLOAT128 Xmm8;
	SETJMP_FLOAT128 Xmm9;
	SETJMP_FLOAT128 Xmm10;
	SETJMP_FLOAT128 Xmm11;
	SETJMP_FLOAT128 Xmm12;
	SETJMP_FLOAT128 Xmm13;
	SETJMP_FLOAT128 Xmm14;
	SETJMP_FLOAT128 Xmm15;
} context;

#else

typedef struct __JUMP_BUFFER
{
	unsigned long Ebp;
	unsigned long Ebx;
	unsigned long Ecx;
	unsigned long Edx;
	unsigned long Esi;
	unsigned long Edi;
	unsigned long Esp;
	unsigned long Eip;
}context;

#endif
#pragma pack ()

#define TLS_SIZE    8

#define FB_RESUME 1
#define FB_SUSPEND 2
#define FB_TERMINATE 3

class Fiber
{
public:
	void Ref()
	{
		refs_++;
	}

	void Unref()
	{
		if (--refs_ == 0)
			free(this);
	}

	void join();

	static void sleep(int ms);
	static void yield();
	static Fiber* Current();

public:
	context m_cntxt;
	int refs_;
	Fiber* m_next;
	Fiber* m_join;
	void* m_tls[TLS_SIZE];
};

template<class T>
class List
{
public:
	List() :
			m_first(NULL), m_last(NULL), m_count(0)
	{
	}

	void put(T* pNew)
	{
		if (m_last)
			m_last->m_next = pNew;
		else
			m_first = pNew;

		m_last = pNew;

		m_count++;
	}

	T* get()
	{
		T* pNow = (T*)m_first;

		if (pNow)
		{
			m_first = (T*)pNow->m_next;
			if (m_first == NULL)
				m_last = NULL;

			pNow->m_next = NULL;

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
	volatile T* m_first;
	volatile T* m_last;
	int m_count;
};

#define FIBER_STACK_SIZE    (65536 * 2)

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
	Fiber* m_locker;
	List<Fiber> m_blocks;
};

class autoLocker
{
public:
	autoLocker(Locker& l) :
			m_l(l)
	{
		m_l.lock();
	}

	~autoLocker()
	{
		m_l.unlock();
	}

private:
	Locker& m_l;
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
	List<Fiber> m_blocks;
};

class CondVar
{
public:
	void wait(Locker& l);
	void notify_one();
	void notify_all();

	int blocked() const
	{
		return m_blocks.count();
	}

private:
	List<Fiber> m_blocks;
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
	List<Fiber> m_blocks;
};

template<class T>
class Queue
{
public:
	void put(T* pNew)
	{
		m_list.put(pNew);
		m_sem.post();
	}

	T* get()
	{
		m_sem.wait();
		return m_list.get();
	}

	T* tryget()
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

class Service;
class AsyncEvent
{
public:
	AsyncEvent(Service* pService);
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

public:
	AsyncEvent* m_next;

private:
	Event weak;
	Service* m_service;
	int m_v;
};

typedef void (*IDLE_PROC)();

class Service
{
public:
	Service();

public:
	static Fiber* CreateFiber(void* (*func)(void *), void *data = NULL,
			int stacksize = FIBER_STACK_SIZE);

	static int tlsAlloc();
	static void* tlsGet(int idx);
	static void tlsPut(int idx, void* v);
	static void tlsFree(int idx);

public:
	void switchtonext();
	void yield();
	static Service* getFiberService();
	static void init();
	static bool hasService();

	IDLE_PROC onIdle(IDLE_PROC proc)
	{
		IDLE_PROC p = m_Idle;
		m_Idle = proc;
		return p;
	}

public:
	Fiber m_main;
	Fiber* m_running;
	Fiber* m_recycle;
	char m_tls[TLS_SIZE];
	List<Fiber> m_resume;
	lockfree<AsyncEvent> m_aEvents;
	List<AsyncEvent> m_yieldList;

	IDLE_PROC m_Idle;
};

}

#endif

