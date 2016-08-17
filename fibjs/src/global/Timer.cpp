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
		SetPrivate("callback", callback);
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

	retVal = new JSTimer(callback, timeout, true);
	return 0;
}

result_t global_base::setTimeout(v8::Local<v8::Function> callback, int32_t timeout, obj_ptr<Timer_base>& retVal)
{
	if (timeout < 1)
		timeout = 1;

	retVal = new JSTimer(callback, timeout);
	return 0;
}

result_t global_base::setImmediate(v8::Local<v8::Function> callback, obj_ptr<Timer_base>& retVal)
{
	retVal = new JSTimer(callback);
	return 0;
}

}
