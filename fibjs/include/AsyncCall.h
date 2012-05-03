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
			func(f), args(a), hr(0)
	{
		q.put(this);
		v8::Unlocker unlocker(isolate);
		wait();
	}

public:
	void (*func)(AsyncCall*);
	void ** args;
	result_t hr;
};

class AsyncLog: public exlib::AsyncEvent
{
public:
	AsyncLog(int priority, std::string msg) :
		m_priority(priority), m_msg(msg)
	{}

public:
	int m_priority;
	std::string m_msg;
};

typedef exlib::lockfree<AsyncLog> AsyncLogQueue;

}

#endif
