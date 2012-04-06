#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <exlib/fiber.h>
#include <exlib/lockfree.h>
#include "utils.h"
#include <string>

namespace fibjs
{

class AsyncCall: public exlib::AsyncEvent
{
public:
	AsyncCall(void ** a, void (*f)(AsyncCall*) = NULL) :
			func(f), args(a), hr(0)
	{
	}

public:
	void (*func)(AsyncCall*);
	void ** args;
	result_t hr;
};

typedef exlib::lockfree<AsyncCall> AsyncQueue;

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
