#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <string>

namespace fibjs
{

typedef exlib::lockfree<asyncEvent> AsyncQueue;
extern AsyncQueue s_acPool;

class AsyncCall: public asyncEvent
{
public:
	AsyncCall(void ** a) :
			args(a)
	{
	}

	int wait()
	{
		if(isset())
			return result();

		v8::Unlocker unlocker(isolate);
		return asyncEvent::wait();
	}

public:
	void ** args;
};

class AsyncCallBack: public AsyncCall
{
public:
	AsyncCallBack(object_base* pThis, void ** a) :
			AsyncCall(a), m_pThis(pThis)
	{
	}

	int wait()
	{
		int hr = AsyncCall::wait();
		m_pThis->extMemory(0);

		return hr;
	}

	virtual void callback();

	virtual int post(int v)
	{
		if (m_pThis->hasTrigger())
			return AsyncCall::post(v);

		delete this;
		return 0;
	}

protected:
	static const char* m_v(const char*& s);
	static const char* m_v(std::string& s)
	{
		return s.c_str();
	}

	template<typename T>
	static T& m_v(T& v)
	{
		return v;
	}

	template<typename T>
	void _trigger(const char* strEvent, T& pv)
	{
		if (m_pThis)
		{
			int hr = result();

			if (hr >= 0)
			{
				v8::Handle<v8::Value> v;

				if (hr != CALL_RETURN_NULL)
					v = ReturnValue(pv);
				else
					v = v8::Null();

				m_pThis->_trigger(strEvent, &v, 1);
			}
			else
				m_pThis->_trigger("error", NULL, 0);
		}

		delete this;
	}

	void _trigger(const char* strEvent)
	{
		if (m_pThis)
		{
			if (result() >= 0)
				m_pThis->_trigger(strEvent, NULL, 0);
			else
				m_pThis->_trigger("error", NULL, 0);
		}

		delete this;
	}

public:
	obj_ptr<object_base> m_pThis;
};

class asyncState: public asyncCallBack
{
public:
	asyncState(exlib::AsyncEvent* ac) :
			m_ac(ac), m_bAsyncState(false), m_state(NULL)
	{
	}

public:
	void set(int (*fn)(asyncState*, int))
	{
		m_state = fn;
	}

	bool is(int (*fn)(asyncState*, int))
	{
		return m_state == fn;
	}

	int done(int v)
	{
		m_state = NULL;
		return v;
	}

	virtual int post(int v)
	{
		result_t hr = v;
		bool bAsyncState = m_bAsyncState;

		if (!bAsyncState)
			m_bAsyncState = true;

		do
		{
			if (hr < 0)
				hr = error(hr);

			if (hr < 0 || !m_state)
			{
				if (bAsyncState && m_ac)
					m_ac->post(hr);

				return end(hr);
			}

			hr = m_state(this, hr);
		} while (hr != CALL_E_PENDDING);

		return hr;
	}

	virtual void invoke()
	{
		post(m_av);
	}

	virtual int apost(int v)
	{
		m_av = v;

		s_acPool.put(this);
		return 0;
	}

	virtual int error(int v)
	{
		return v;
	}

	virtual int end(int v)
	{
		delete this;
		return v;
	}

private:
	exlib::AsyncEvent* m_ac;
	bool m_bAsyncState;
	int m_av;
	int (*m_state)(asyncState*, int);
};

class AsyncLog: public asyncEvent
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
