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
		m_isolate = Isolate::now()->m_isolate;
		m_callback.Reset(m_isolate, callback);

		if (m_timeout < 1)
			m_timeout = 1;

		sleep();
		Ref();
	}

	Timer(v8::Local<v8::Function> callback) :
		m_timeout(0), m_repeat(false), m_cancel(false)
	{
		m_isolate = Isolate::now()->m_isolate;
		m_callback.Reset(m_isolate, callback);

		resume();
		Ref();
	}

	~Timer()
	{
		m_callback.Reset();
	}

public:
	// object_base
	virtual bool isJSObject()
	{
		return true;
	}

public:
	// Timer_base
	virtual result_t clear()
	{
		m_cancel = true;
		exlib::Fiber::cancel_sleep(this);
		return 0;
	}

public:
	// exlib::Task_base
	virtual void suspend()
	{
	}

	virtual void resume()
	{
		syncCall(_callback, this);
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
				v8::Local<v8::Function>::New(m_isolate, m_callback)->Call(wrap(), 0, NULL);
			}

			if (m_repeat && !m_cancel)
				sleep();
			else
			{
				dispose();
				Unref();
			}
		} else
		{
			dispose();
			Unref();
		}
	}

	static void _callback(Timer* pThis)
	{
		pThis->callback();
	}

private:
	v8::Isolate* m_isolate;
	v8::Persistent<v8::Function> m_callback;
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
