#pragma once

#include <string>
#include <exlib/include/fiber.h>
#include "utils.h"
#include "Runtime.h"

namespace fibjs {

class AsyncEvent : public exlib::Task_base {
public:
    AsyncEvent(Isolate* isolate = NULL)
        : m_isolate(isolate)
        , m_async(false)
    {
    }

    virtual ~AsyncEvent()
    {
    }

public:
    virtual void resume()
    {
        apost(0);
    }

public:
    void sync(Isolate* isolate)
    {
        isolate->Ref();
        isolate->m_jobs.putTail(this);
        isolate->m_sem.post();
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
        assert(m_isolate);
        return m_isolate;
    }

    bool isAsync() const
    {
        return m_async;
    }

    bool isSync() const
    {
        return !m_async;
    }

    void setAsync()
    {
        m_async = true;
    }

public:
    std::vector<Variant> m_ctx;
    obj_ptr<object_base> m_ctxo;

protected:
    Isolate* m_isolate;

private:
    bool m_async;
};

class AsyncCall : public AsyncEvent {
public:
    AsyncCall(void** a)
        : AsyncEvent(Isolate::current())
        , args(a)
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

    int32_t check_result(int32_t hr)
    {
        if (hr == CALL_E_NOSYNC) {
            Isolate::LeaveJsScope _rt(m_isolate);
            invoke();
            weak.wait();
        } else if (hr == CALL_E_LONGSYNC || hr == CALL_E_GUICALL) {
            async(hr);

            if (!weak.isSet()) {
                Isolate::LeaveJsScope _rt(m_isolate);
                weak.wait();
            }
        } else
            return hr;

        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void** args;

private:
    exlib::string m_error;
    int32_t m_v;
};

class CAsyncCall : public AsyncEvent {
public:
    CAsyncCall(void** a, Isolate* isolate = NULL)
        : AsyncEvent(isolate)
        , args(a)
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

    int32_t check_result(int32_t hr)
    {
        if (hr == CALL_E_NOSYNC)
            invoke();
        else if (hr == CALL_E_LONGSYNC || hr == CALL_E_GUICALL)
            async(hr);
        else
            return hr;

        weak.wait();
        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error);

        return m_v;
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
        , m_next(NULL)
    {
        setAsync();
    }

public:
    class ASResult {
    public:
        ASResult(AsyncState* as, int32_t r)
            : m_as(as)
            , m_r(r)
        {
        }

        operator AsyncState*() const
        {
            return m_as;
        }

        operator int32_t() const
        {
            return m_r;
        }

    private:
        AsyncState* m_as;
        int32_t m_r;
    };

public:
    ASResult next(int32_t (*fn)(AsyncState*, int32_t), int32_t r = 0)
    {
        m_next = fn;
        return ASResult(this, r);
    }

    ASResult next(int32_t r = 0)
    {
        m_next = NULL;
        return ASResult(this, r);
    }

    bool at(int32_t (*fn)(AsyncState*, int32_t))
    {
        return m_state == fn;
    }

public:
    virtual int32_t post(int32_t v)
    {
        result_t hr = v;
        bool bAsyncState = m_bAsyncState;

        if (!bAsyncState)
            m_bAsyncState = true;

        do {
            if (hr < 0)
                hr = error(hr);

            if (hr < 0 || !m_next) {
                if (bAsyncState && m_ac)
                    m_ac->post(hr);

                delete this;
                return hr;
            }

            m_state = m_next;
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

public:
    virtual void resume()
    {
        apost(0);
    }

private:
    AsyncEvent* m_ac;
    bool m_bAsyncState;
    int32_t m_v;
    int32_t (*m_state)(AsyncState*, int32_t);
    int32_t (*m_next)(AsyncState*, int32_t);
};

#define ON_STATE(cls, fn)                            \
    static int32_t fn(AsyncState* pState, int32_t n) \
    {                                                \
        return ((cls*)pState)->_##fn(n);             \
    }                                                \
    int32_t _##fn(int32_t n)

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
void asyncCall(T func, T1 v, int32_t mode = CALL_E_NOSYNC)
{
    (new AsyncFunc<T, T1>(func, v))->async(mode);
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

class NType;
class AsyncCallBack : public AsyncEvent {
public:
    AsyncCallBack(v8::Local<v8::Function> cb, object_base* pThis = NULL);

    ~AsyncCallBack()
    {
        m_isolate->Unref();
        m_cb.Reset();
    }

public:
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

    int32_t check_result(int32_t hr, const v8::FunctionCallbackInfo<v8::Value>& args);

protected:
    void fillRetVal(std::vector<v8::Local<v8::Value>>& args, object_base* obj);
    void fillRetVal(std::vector<v8::Local<v8::Value>>& args, NType* v);

    template <typename T>
    void fillRetVal(std::vector<v8::Local<v8::Value>>& args, obj_ptr<T>& v)
    {
        fillRetVal(args, (T*)v);
    }

    template <typename T>
    void fillRetVal(std::vector<v8::Local<v8::Value>>& args, T& v)
    {
        args.push_back(GetReturnValue(m_isolate->m_isolate, v));
    }

    virtual void fillArguments(std::vector<v8::Local<v8::Value>>& args)
    {
    }

    static result_t syncFunc(AsyncCallBack* pThis);

protected:
    obj_ptr<object_base> m_pThis;
    v8::Global<v8::Function> m_cb;
    v8::Global<v8::Value> m_cb_err;

private:
    exlib::string m_error;
    int32_t m_v;
};
}
