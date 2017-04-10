#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <string>
#include <exlib/include/fiber.h>
#include "utils.h"
#include "Runtime.h"

namespace fibjs {

class AsyncEvent : public exlib::linkitem {
public:
    virtual ~AsyncEvent()
    {
    }

public:
    void sync(Isolate* isolate)
    {
        isolate->m_pendding.inc();
        isolate->m_jobs.put(this);
    }

    virtual result_t js_invoke()
    {
        return 0;
    }

    void async(int32_t type);
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

    virtual Isolate* isolate()
    {
        assert(false);
        return NULL;
    }
};

class AsyncCall : public AsyncEvent {
public:
    AsyncCall(void** a)
        : args(a)
    {
        m_isolate = Isolate::current();
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
        if (!weak.isSet()) {
            Isolate::rt _rt(m_isolate);
            weak.wait();
        }

        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
    }

    int32_t async_wait()
    {
#ifdef _WIN32
        async(CALL_E_NOSYNC);
        return wait();
#else
        {
            Isolate::rt _rt(m_isolate);
            invoke();
            weak.wait();
        }

        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
#endif
    }

    virtual Isolate* isolate()
    {
        assert(m_isolate);
        return m_isolate;
    }

protected:
    exlib::Event weak;
    void** args;

private:
    Isolate* m_isolate;
    exlib::string m_error;
    int32_t m_v;
};

class CAsyncCall : public AsyncEvent {
public:
    CAsyncCall(void** a)
        : args(a)
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

    int32_t async_wait()
    {
#ifdef _WIN32
        async(CALL_E_NOSYNC);
        return wait();
#else
        invoke();
        weak.wait();

        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
#endif
    }

protected:
    exlib::Event weak;
    void** args;

private:
    exlib::string m_error;
    int32_t m_v;
};

class AsyncState : public AsyncEvent {
public:
    AsyncState(AsyncEvent* ac)
        : m_ac(ac)
        , m_bAsyncState(false)
        , m_state(NULL)
    {
    }

public:
    void set(int32_t (*fn)(AsyncState*, int32_t))
    {
        m_state = fn;
    }

    bool is(int32_t (*fn)(AsyncState*, int32_t))
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

        do {
            if (hr < 0)
                hr = error(hr);

            if (hr < 0 || !m_state) {
                if (bAsyncState && m_ac)
                    m_ac->post(hr);

                delete this;
                return hr;
            }

            hr = m_state(this, hr);
        } while (hr != CALL_E_PENDDING);

        return hr;
    }

    virtual void invoke()
    {
        post(m_v);
    }

    virtual void apost(int32_t v)
    {
        m_bAsyncState = true;
        m_v = v;
        async(CALL_E_NOSYNC);
    }

    virtual int32_t error(int32_t v)
    {
        return v;
    }

    virtual Isolate* isolate()
    {
        return m_ac->isolate();
    }

private:
    AsyncEvent* m_ac;
    bool m_bAsyncState;
    int32_t m_v;
    int32_t (*m_state)(AsyncState*, int32_t);
};

template <typename T, typename T1>
class AsyncFunc : public AsyncEvent {
public:
    AsyncFunc(T func, T1 v)
        : m_func(func)
        , m_v(v)
    {
    }

    virtual void invoke()
    {
        m_func(m_v);
        delete this;
    }

    virtual result_t js_invoke()
    {
        result_t hr = m_func(m_v);
        delete this;
        return hr;
    }

private:
    T m_func;
    T1 m_v;
};

template <typename T, typename T1>
void asyncCall(T func, T1 v)
{
    (new AsyncFunc<T, T1>(func, v))->async(CALL_E_NOSYNC);
}

template <typename T, typename T1>
void syncCall(Isolate* isolate, T func, T1 v)
{
    (new AsyncFunc<T, T1>(func, v))->sync(isolate);
}

template <typename T>
class _at {
public:
    _at(T& v)
        : m_v(v)
    {
    }

    T& value()
    {
        return m_v;
    }

private:
    T m_v;
};

template <typename T>
class _at<T*> {
public:
    _at(T* v)
        : m_v(v)
    {
    }

    T* value()
    {
        return m_v;
    }

private:
    obj_ptr<T> m_v;
};

class AsyncCallBack : public AsyncEvent,
                      public exlib::Task_base {
public:
    AsyncCallBack(v8::Local<v8::Function> cb)
    {
        m_isolate = Isolate::current();
        m_isolate->m_pendding.inc();
        m_cb.Reset(m_isolate->m_isolate, cb);
    }

    AsyncCallBack(object_base* pThis, v8::Local<v8::Function> cb);

    ~AsyncCallBack()
    {
        m_isolate->m_pendding.dec();
        m_cb.Reset();
    }

public:
    virtual void suspend()
    {
    }

    virtual void suspend(exlib::spinlock& lock)
    {
        lock.unlock();
    }

    virtual void resume()
    {
        async(m_v);
    }

public:
    int32_t callback(int32_t v)
    {
        if (v == CALL_E_EXCEPTION)
            m_error = Runtime::errMessage();

        m_v = v;
        syncCall(m_isolate, syncFunc, this);

        return 0;
    }

    void async_call(int32_t v);
    virtual int32_t post(int32_t v);

    virtual Isolate* isolate()
    {
        assert(m_isolate);
        return m_isolate;
    }

protected:
    virtual v8::Local<v8::Value> getValue()
    {
        return v8::Undefined(m_isolate->m_isolate);
    }

    static result_t syncFunc(AsyncCallBack* pThis);

protected:
    obj_ptr<object_base> m_pThis;
    v8::Persistent<v8::Function> m_cb;

private:
    Isolate* m_isolate;
    exlib::string m_error;
    int32_t m_v;
};
}

#endif
