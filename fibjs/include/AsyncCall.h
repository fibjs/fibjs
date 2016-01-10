#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <string>
#include <exlib/include/fiber.h>

namespace fibjs
{

class AsyncEvent: public exlib::linkitem
{
public:
    virtual ~AsyncEvent()
    {}

public:
    void sync(Isolate* isolate)
    {
        isolate->m_jobs.put(this);
    }

    virtual void js_invoke()
    {
    }

    void async();
    virtual void invoke()
    {
    }

    virtual int32_t post(int32_t v)
    {
        return 0;
    }

    virtual void apost(int32_t v)
    {
        post(v);
    }
};

class AsyncCall: public AsyncEvent
{
public:
    AsyncCall(void **a) :
        args(a)
    {
    }

    virtual int32_t post(int32_t v)
    {
        if (v == CALL_E_EXCEPTION)
            m_error = Runtime::errMessage();

        m_v = v;
        weak.set();

        return 0;
    }

    int32_t wait()
    {
        if (!weak.isSet())
        {
            Isolate::rt _rt;
            weak.wait();
        }

        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void **args;

private:
    std::string m_error;
    int32_t m_v;
};

class CAsyncCall: public AsyncEvent
{
public:
    CAsyncCall(void **a) :
        args(a)
    {
    }

    virtual int32_t post(int32_t v)
    {
        if (v == CALL_E_EXCEPTION)
            m_error = Runtime::errMessage();

        m_v = v;
        weak.set();

        return 0;
    }

    int32_t wait()
    {
        weak.wait();
        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void **args;

private:
    std::string m_error;
    int32_t m_v;
};

class AsyncState: public AsyncEvent
{
public:
    AsyncState(AsyncEvent *ac) :
        m_ac(ac), m_bAsyncState(false), m_state(NULL)
    {
    }

public:
    void set(int32_t (*fn)(AsyncState *, int32_t))
    {
        m_state = fn;
    }

    bool is(int32_t (*fn)(AsyncState *, int32_t))
    {
        return m_state == fn;
    }

    int32_t done(int32_t v = 0)
    {
        m_state = NULL;
        return v;
    }

    virtual int32_t post(int32_t v)
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
        post(m_v);
    }

    virtual void apost(int32_t v)
    {
        m_v = v;
        async();
    }

    virtual int32_t error(int32_t v)
    {
        return v;
    }

private:
    AsyncEvent *m_ac;
    bool m_bAsyncState;
    int32_t m_v;
    int32_t (*m_state)(AsyncState *, int32_t);
};

template<typename T, typename T1>
class AsyncFunc: public AsyncEvent
{
public:
    AsyncFunc(T func, T1 v) :
        m_func(func), m_v(v)
    {
    }

    virtual void invoke()
    {
        m_func(m_v);
        delete this;
    }

    virtual void js_invoke()
    {
        invoke();
    }

private:
    T m_func;
    T1 m_v;
};

template<typename T, typename T1>
void asyncCall(T func, T1 v)
{
    (new AsyncFunc<T, T1>(func, v))->async();
}

template<typename T, typename T1>
void syncCall(Isolate* isolate, T func, T1 v)
{
    (new AsyncFunc<T, T1>(func, v))->sync(isolate);
}

}

#endif
