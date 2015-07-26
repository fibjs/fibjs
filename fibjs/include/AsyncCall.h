#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <string>
#include <exlib/include/thread.h>

namespace fibjs
{

class AsyncCall: public AsyncEvent
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

        return AsyncEvent::post(v);
    }

    int wait()
    {
        int r;

        if (isSet())
            r = result();
        else
        {
            Isolate::rt _rt;
            r = AsyncEvent::wait();
        }

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
    asyncState(AsyncEvent *ac) :
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

    int done(int v = 0)
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

        async();
        return 0;
    }

    virtual int error(int v)
    {
        return v;
    }

private:
    AsyncEvent *m_ac;
    bool m_bAsyncState;
    int m_av;
    int (*m_state)(asyncState *, int);
};

}

#endif
