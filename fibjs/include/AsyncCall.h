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
    AsyncCall(void **a) :
        args(a)
    {
    }

    virtual int post(int v)
    {
        if (v == CALL_E_EXCEPTION)
            m_error = Runtime::errMessage();

        return asyncEvent::post(v);
    }

    int wait()
    {
        if (isSet())
            return result();

        v8::Unlocker unlocker(isolate);
        int r = asyncEvent::wait();

        if (r == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return r;
    }

public:
    void **args;
    std::string m_error;
};

class asyncState: public asyncCallBack
{
public:
    asyncState(exlib::AsyncEvent *ac) :
        m_ac(ac), m_bAsyncState(false), m_state(NULL)
    {
    }

public:
    void set(int (*fn)(asyncState *, int))
    {
        m_state = fn;
    }

    bool is(int (*fn)(asyncState *, int))
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

                delete this;
                return hr;
            }

            hr = m_state(this, hr);
        }
        while (hr != CALL_E_PENDDING);

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

private:
    exlib::AsyncEvent *m_ac;
    bool m_bAsyncState;
    int m_av;
    int (*m_state)(asyncState *, int);
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
