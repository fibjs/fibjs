#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include "utils.h"
#include <exlib/fiber.h>
#include <exlib/lockfree.h>
#include <string>

namespace fibjs
{

class AsyncCall;
typedef exlib::lockfree<AsyncCall> AsyncQueue;

class AsyncCall: public exlib::AsyncEvent
{
public:
	AsyncCall(AsyncQueue& q, void ** a, void (*f)(AsyncCall*) = NULL) :
			func(f), args(a)
	{
		q.put(this);
	}

	int wait()
	{
		v8::Unlocker unlocker(isolate);
		return exlib::AsyncEvent::wait();
	}

public:
	void (*func)(AsyncCall*);
	void ** args;
};

class object_base;
class AsyncCallBack: public AsyncCall
{
public:
	AsyncCallBack(AsyncQueue& q, object_base* pThis, void ** a, void (*f)(AsyncCall*) = NULL) :
			AsyncCall(q, a, f), m_pThis(pThis)
	{
	}

	virtual void invoke();
	virtual void callback() = 0;

public:
	object_base* m_pThis;
};

class AsyncLog: public exlib::AsyncEvent
{
public:
	AsyncLog(int priority, std::string msg) :
			m_priority(priority), m_msg(msg)
	{
	}

public:
	int m_priority;
	std::string m_msg;
};

typedef exlib::lockfree<AsyncLog> AsyncLogQueue;

}

#endif
