/*
 * Timer.h
 *
 *  Created on: Sun 14, 2016
 *      Author: lion
 */

#include "ifs/Timer.h"
#include "Fiber.h"
#include <vector>

#ifndef TIMER_H_
#define TIMER_H_

namespace fibjs
{

class Timer : public Timer_base,
	public exlib::Task_base
{
public:
	Timer(int32_t timeout = 0, bool repeat = false) :
		m_timeout(timeout), m_repeat(repeat), m_cancel(0)
	{
		holder();

		if (timeout > 0)
			sleep();
		else
			resume();

		Ref();
	}

public:
	// Timer_base
	virtual result_t clear()
	{
		if (m_cancel.CompareAndSwap(0, 1))
			exlib::Fiber::cancel_sleep(this);

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

public:
	virtual void on_timer()
	{
	}

private:
	void callback()
	{
		if (!m_cancel)
		{
			{
				JSFiber::scope s;
				on_timer();
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
	exlib::atomic m_cancel;
};

}

#endif
