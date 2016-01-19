/*
 * Timer.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: lion
 */

#include "ifs/global.h"
#include "Fiber.h"

namespace fibjs
{

class Timer : public Timer_base,
	public exlib::Task_base
{
public:
	Timer(v8::Local<v8::Function> callback, int32_t timeout, bool repeat = false) :
		m_timeout(timeout), m_repeat(repeat), m_cancel(false)
	{
		wrap()->SetHiddenValue(v8::String::NewFromUtf8(holder()->m_isolate, "callback"), callback);

		if (m_timeout < 1)
			m_timeout = 1;

		sleep();
		Ref();
	}

	Timer(v8::Local<v8::Function> callback) :
		m_timeout(0), m_repeat(false), m_cancel(false)
	{
		wrap()->SetHiddenValue(v8::String::NewFromUtf8(holder()->m_isolate, "callback"), callback);

		resume();
		Ref();
	}

public:
	// Timer_base
	virtual result_t clear()
	{
		if (!m_cancel)
		{
			m_cancel = true;
			exlib::Fiber::cancel_sleep(this);
		}

		return 0;
	}

public:
	// exlib::Task_base
	virtual void suspend()
	{
	}

	virtual void suspend(exlib::spinlock& lock)
	{
		lock.unlock();
	}

	virtual void resume()
	{
		syncCall(holder(), _callback, this);
	}

	void sleep()
	{
		exlib::Fiber::sleep(m_timeout, this);
	}

private:
	void callback()
	{
		if (!m_cancel)
		{
			{
				JSFiber::scope s;
				v8::Local<v8::Value> v = wrap()->GetHiddenValue(v8::String::NewFromUtf8(holder()->m_isolate, "callback"));

				if (!v.IsEmpty() && v->IsFunction())
					v8::Local<v8::Function>::Cast(v)->Call(wrap(), 0, NULL);
			}

			if (m_repeat && !m_cancel)
				sleep();
			else
				Unref();
		} else
			Unref();
	}

	static void _callback(Timer* pThis)
	{
		pThis->callback();
	}

private:
	int32_t m_timeout;
	bool m_repeat;
	bool m_cancel;
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
	retVal = new Timer(callback, timeout, true);
	return 0;
}

result_t global_base::setTimeout(v8::Local<v8::Function> callback, int32_t timeout, obj_ptr<Timer_base>& retVal)
{
	retVal = new Timer(callback, timeout);
	return 0;
}

result_t global_base::setImmediate(v8::Local<v8::Function> callback, obj_ptr<Timer_base>& retVal)
{
	retVal = new Timer(callback);
	return 0;
}

}
