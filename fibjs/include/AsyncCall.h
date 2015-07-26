#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <string>
#include <exlib/include/thread.h>

namespace fibjs
{

class AsyncEvent: public exlib::linkitem
{
public:
    virtual ~AsyncEvent()
    {}

public:
    void sync(int32_t v = 0);
    virtual void js_invoke()
    {
    }

    void async(int32_t v = 0);
    virtual void invoke()
    {
    }

public:
    virtual int post(int v)
    {
        m_v = v;
        weak.set();

        return 0;
    }

    int wait()
    {
        weak.wait();
        return m_v;
    }

    bool isSet()
    {
        return weak.isSet();
    }

    int result()
    {
        return m_v;
    }

    void sleep(int ms);

private:
    exlib::Event weak;
    int32_t m_v;
};

class AsyncCall: public AsyncEvent
{
public:
    AsyncCall(void **a) :
        args(a), invoke_once(0)
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
    int32_t invoke_once;
};

class AsyncState: public AsyncEvent
{
public:
    AsyncState(AsyncEvent *ac) :
        m_ac(ac), m_bAsyncState(false), m_state(NULL)
    {
    }

public:
    void set(int (*fn)(AsyncState *, int))
    {
        m_state = fn;
    }

    bool is(int (*fn)(AsyncState *, int))
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
        post(result());
    }

    virtual int error(int v)
    {
        return v;
    }

private:
    AsyncEvent *m_ac;
    bool m_bAsyncState;
    int (*m_state)(AsyncState *, int);
};

template<typename T, typename T1>
void AsyncClose(T hd, T1 func)
{
    class _AsyncClose: public AsyncEvent
    {
    public:
        _AsyncClose(T hd, T1 func) :
            m_hd(hd), m_func(func)
        {
        }

        virtual void invoke()
        {
            m_func(m_hd);
            delete this;
        }

    private:
        T m_hd;
        T1 m_func;
    };

    (new _AsyncClose(hd, func))->async();
}

template<typename T, typename T1>
void DelayClose(T hd, T1 func)
{
    class _DelayClose: public AsyncEvent
    {
    public:
        _DelayClose(T hd, T1 func) :
            m_hd(hd), m_func(func)
        {
        }

        virtual void js_invoke()
        {
            m_func(m_hd);
            delete this;
        }

    private:
        T m_hd;
        T1 m_func;
    };

    (new _DelayClose(hd, func))->sync();
}

}

#endif
