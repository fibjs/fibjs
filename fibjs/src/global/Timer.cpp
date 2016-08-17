/*
 * Timer.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/global.h"
#include "Timer.h"

namespace fibjs
{

class JSTimer : public Timer
{
public:
	JSTimer(v8::Local<v8::Function> callback, int32_t timeout = 0, bool repeat = false) :
		Timer(timeout, repeat)
	{
		holder();

		SetPrivate("callback", callback);
	}

public:
	virtual void resume()
	{
		syncCall(holder(), _callback, this);
	}

public:
	virtual void on_timer()
	{
		JSFiber::scope s;

		v8::Local<v8::Value> v = GetPrivate("callback");

		if (v->IsFunction())
			v8::Local<v8::Function>::Cast(v)->Call(wrap(), 0, NULL);
	}
};

result_t global_base::clearInterval(Timer_base* t)
{
	return t->clear();
}

result_t global_base::clearTimeout(Timer_base* t)
{
	return t->clear();
}

result_t global_base::clearImmediate(Timer_base* t)
{
	return t->clear();
}

result_t global_base::setInterval(v8::Local<v8::Function> callback, int32_t timeout, obj_ptr<Timer_base>& retVal)
{
	if (timeout < 1)
		timeout = 1;

	obj_ptr<Timer> timer = new JSTimer(callback, timeout, true);
	timer->sleep();
	retVal = timer;

	return 0;
}

result_t global_base::setTimeout(v8::Local<v8::Function> callback, int32_t timeout, obj_ptr<Timer_base>& retVal)
{
	if (timeout < 1)
		timeout = 1;

	obj_ptr<Timer> timer = new JSTimer(callback, timeout);
	timer->sleep();
	retVal = timer;

	return 0;
}

result_t global_base::setImmediate(v8::Local<v8::Function> callback, obj_ptr<Timer_base>& retVal)
{
	obj_ptr<Timer> timer = new JSTimer(callback);
	timer->sleep();
	retVal = timer;

	return 0;
}

}
