#pragma once

#include <atomic>
#include <string>
#include <functional>
#include <exlib/include/fiber.h>
#include "utils.h"
#include "Runtime.h"

#if defined(FIBJS_ASYNC_STATE_CHECK)
#include <stdio.h>
#include <stdlib.h>
// Stack trace helper from exlib: backtrace(3) on glibc, DbgHelp on Windows,
// no-op on libcs that do not provide execinfo.h (musl/Alpine, Android bionic).
// Including <execinfo.h> unconditionally breaks those targets at compile time.
#include <exlib/include/ex_assert.h>

// Return address of the direct caller, used to point at the violating site.
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#define FIBJS_ASYNC_STATE_CALLER() ((void*)_ReturnAddress())
#else
#define FIBJS_ASYNC_STATE_CALLER() __builtin_return_address(0)
#endif
#endif

namespace fibjs {

class AsyncEvent : public exlib::Task_base {
public:
    enum kCallType {
        kAsyncCallEvent,
        kAsyncCall,
        kCAsyncCall,
        kAsyncCallBack
    };

private:
    enum kStateType {
        kStateSync,
        kStateAsync
    };

public:
    AsyncEvent(Isolate* isolate = NULL);
    virtual ~AsyncEvent();

public:
    virtual void resume()
    {
        apost(0);
    }

public:
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
        ex_assert(m_isolate);
        return m_isolate;
    }

    bool isAsync() const
    {
        return m_state == kStateAsync;
    }

    bool isSync() const
    {
        return m_state == kStateSync;
    }

    virtual kCallType callType() const
    {
        return kAsyncCallEvent;
    }

    void setAsync()
    {
        m_state = kStateAsync;
    }

public:
    std::vector<Variant> m_ctx;
    obj_ptr<object_base> m_ctxo;

protected:
    Isolate* m_isolate;

private:
    kStateType m_state;
};

class AsyncCall : public AsyncEvent {
public:
    AsyncCall(void** a, Isolate* isolate)
        : AsyncEvent(isolate)
        , args(a)
    {
    }

    virtual kCallType callType() const override
    {
        return kAsyncCall;
    }

    virtual int32_t post(int32_t v)
    {
        if (v == CALL_E_EXCEPTION) {
            m_error_code = Runtime::errCode();
            m_error_type = Runtime::errType();
            m_error = Runtime::errMessage();
        }

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

            if (_rt.is_terminating())
                m_v = CALL_E_TIMEOUT;
        } else if (hr == CALL_E_LONGSYNC || hr == CALL_E_GUICALL) {
            async(hr);

            if (!weak.isSet()) {
                Isolate::LeaveJsScope _rt(m_isolate);
                weak.wait();

                if (_rt.is_terminating())
                    m_v = CALL_E_TIMEOUT;
            }
        } else if (hr < 0)
            return hr;
        else
            m_v = hr;

        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(m_error_type, m_error_code, m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void** args;

private:
    exlib::string m_error;
    result_t m_error_code = 0;
    ErrorType m_error_type = kError;
    int32_t m_v;
};

class CAsyncCall : public AsyncEvent {
public:
    CAsyncCall(void** a, Isolate* isolate)
        : AsyncEvent(isolate)
        , args(a)
    {
    }

    virtual kCallType callType() const override
    {
        return kCAsyncCall;
    }

    virtual int32_t post(int32_t v)
    {
        if (v == CALL_E_EXCEPTION) {
            m_error_code = Runtime::errCode();
            m_error_type = Runtime::errType();
            m_error = Runtime::errMessage();
        }

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
            Runtime::setError(m_error_type, m_error_code, m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void** args;

private:
    exlib::string m_error;
    result_t m_error_code = 0;
    ErrorType m_error_type = kError;
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
        ASResult(AsyncEvent* as, int32_t r)
            : m_as(as)
            , m_r(r)
        {
        }

#if defined(FIBJS_ASYNC_STATE_CHECK)
        // 检测模式：next() 交出的是状态锁（票），只能当 AsyncEvent 使用。
        // 任何把 next() 的结果当 AsyncState* 用的站点都会在编译期暴露出来。
        operator AsyncEvent*() const
        {
            return m_as;
        }
#else
        operator AsyncState*() const
        {
            return static_cast<AsyncState*>(m_as);
        }
#endif

        operator int32_t() const
        {
            return m_r;
        }

    private:
        AsyncEvent* m_as;
        int32_t m_r;
    };

public:
    ASResult next(int32_t (*fn)(AsyncState*, int32_t), int32_t r = 0)
    {
        m_next = fn;
        return issue(r);
    }

    ASResult next(int32_t r = 0)
    {
        m_next = NULL;
        return issue(r);
    }

private:
    ASResult issue(int32_t r)
    {
#if defined(FIBJS_ASYNC_STATE_CHECK)
        // 状态锁：发票。next() 只在机器已被派发（处于处理态）后使用：
        // ++expect 必须追平 epoch。构造期设置入口状态请用 init()。
        if (++m_cont.expect != m_epoch.load(std::memory_order_relaxed))
            state_lock_violation("next() while the machine was not in the processing phase (set the entry state with init() from a constructor; a second arm before delivery is also a violation)", r, FIBJS_ASYNC_STATE_CALLER());
        return ASResult(&m_cont, r);
#else
        return ASResult(this, r);
#endif
    }

public:
    bool at(int32_t (*fn)(AsyncState*, int32_t))
    {
        return m_state == fn;
    }

public:
    // 设定状态机的入口状态（仅构造期、首次派发前使用）。
    // 与 next() 的区别：只声明“首次进入时执行的状态函数”，不发票、不发布代理。
    void init(int32_t (*fn)(AsyncState*, int32_t))
    {
#if defined(FIBJS_ASYNC_STATE_CHECK)
        if (m_epoch.load(std::memory_order_relaxed) != 0)
            state_lock_violation("init() must be called before the machine is dispatched", 0, FIBJS_ASYNC_STATE_CALLER());
#endif
        m_next = fn;
    }

public:
    virtual int32_t post(int32_t v)
    {
#if defined(FIBJS_ASYNC_STATE_CHECK)
        // 状态锁：初始派发是唯一合法的直连 post —— 相位必须正好从 0 走到 1。
        if (++m_epoch != 1)
            state_lock_violation("non-initial post() (direct dispatch into a machine that has already been dispatched)", v, FIBJS_ASYNC_STATE_CALLER());
#endif
        return post_(v);
    }

protected:
    int32_t post_(int32_t v)
    {
        result_t hr = v;
        bool bAsyncState = m_bAsyncState;

        if (!bAsyncState)
            m_bAsyncState = true;

        do {
#if defined(FIBJS_ASYNC_STATE_CHECK)
            // 同步完成/同步跳转：进入错误处理或下一个状态函数之前，若本事务发出的
            // 票还没有被消费（expect == epoch），说明它不可能再有人来消费
            // （同步完成的操作不会回投），就地消费掉（等价一次同步回投）。
            if (m_cont.expect.load(std::memory_order_relaxed) == m_epoch.load(std::memory_order_relaxed))
                ++m_epoch;
#endif
            if (hr < 0)
                hr = error(hr);

#if defined(FIBJS_ASYNC_STATE_CHECK)
            // error() 内可能再次发牌（同步跳转），执行下一个状态函数之前同样就地消费
            if (m_cont.expect.load(std::memory_order_relaxed) == m_epoch.load(std::memory_order_relaxed))
                ++m_epoch;
#endif

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

public:
    virtual void invoke()
    {
        post_(m_v);
    }

    virtual void apost(int32_t v)
    {
#if defined(FIBJS_ASYNC_STATE_CHECK)
        // 状态锁：初始派发（异步形态），同 post()。
        if (++m_epoch != 1)
            state_lock_violation("non-initial apost() (direct async dispatch into a machine that has already been dispatched)", v, FIBJS_ASYNC_STATE_CALLER());
#endif
        apost_(v);
    }

    virtual int32_t error(int32_t v)
    {
        return v;
    }

protected:
    void apost_(int32_t v)
    {
        m_bAsyncState = true;
        m_v = v;

        async(CALL_E_NOSYNC);
    }

public:
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

#if defined(FIBJS_ASYNC_STATE_CHECK)
public:
    // ---- 状态锁（仅检测模式） ------------------------------------------
    // 相位只在三处推进：初始化 post()/apost()、next() 发票、proxy 回投；
    // _post() 只消费处理态，不自增。任一关系式不成立立即终止现场
    // （打印相位/调用点/栈后 abort），不做任何兜底。
    //   初始化    if (++epoch != 1) assert            —— post()/apost()
    //   next()    if (++expect != epoch) assert      —— 发票（构造期用 init()）
    //   回投       if (++epoch != expect + 1) assert
    class Continuation : public AsyncEvent {
    public:
        Continuation(AsyncState* machine)
            : m_owner(machine)
        {
            setAsync();
        }

        virtual void apost(int32_t v) override;
        virtual int32_t post(int32_t v) override;
        virtual void invoke() override
        {
            // proxy 本身永远不会被投递进队列；被投递 = 有人对票做了裸 async()
            m_owner->state_lock_violation("continuation was queued directly (raw async() on the proxy)", 0, FIBJS_ASYNC_STATE_CALLER());
        }

        virtual Isolate* isolate() override
        {
            return m_owner->isolate();
        }

    private:
        void deliver(int32_t v, bool async);

    public:
        AsyncState* m_owner;               // 宿主机器（构造时写入，见 §2）
        std::atomic<int32_t> expect { 0 }; // 票面相位：next() 推进，回投时比对
    };

private:
    void state_lock_violation(const char* what, int32_t v, void* caller);

    std::atomic<int32_t> m_epoch { 0 };
    Continuation m_cont { this };
#endif
};

#if defined(FIBJS_ASYNC_STATE_CHECK)
inline void AsyncState::Continuation::deliver(int32_t v, bool async)
{
    AsyncState* machine = m_owner;

    // 票面校验与推进是同一个原子操作，不存在“检查—使用”时间差
    if (++machine->m_epoch != expect.load() + 1)
        machine->state_lock_violation("continuation returned while the machine had moved on (duplicate/late/stray delivery)", v, FIBJS_ASYNC_STATE_CALLER());

    if (async)
        machine->apost_(v);
    else
        machine->post_(v);
}

inline int32_t AsyncState::Continuation::post(int32_t v)
{
    deliver(v, false);
    return 0;
}

inline void AsyncState::Continuation::apost(int32_t v)
{
    deliver(v, true);
}

inline void AsyncState::state_lock_violation(const char* what, int32_t v, void* caller)
{
    fprintf(stderr, "\n[fibjs] AsyncState phase lock violation: %s\n", what);
    fprintf(stderr, "  machine=%p epoch=%d expect=%d state=%p next=%p value=%d caller=%p\n",
        (void*)this, m_epoch.load(std::memory_order_relaxed), m_cont.expect.load(),
        (void*)m_state, (void*)m_next, v, caller);
    fprintf(stderr, "  violating stack:\n");
    ex_print_stack_trace();
    fflush(stderr);
    abort();
}
#endif

#define ON_STATE(cls, fn)                            \
    static int32_t fn(AsyncState* pState, int32_t n) \
    {                                                \
        return ((cls*)pState)->_##fn(n);             \
    }                                                \
    int32_t _##fn(int32_t n)

void async(std::function<void(void)> func, int32_t mode = CALL_E_NOSYNC);

template <typename T>
class _at {
public:
    _at(T& v)
        : m_v(v)
    {
    }

    T& c_value()
    {
        return m_v;
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

    T* c_value()
    {
        return m_v;
    }

    T* value()
    {
        return m_v;
    }

private:
    obj_ptr<T> m_v;
};

template <typename T>
class _at<std::vector<T>> {
public:
    _at(std::vector<T>& v)
        : m_v(std::move(v))
    {
    }

    std::vector<T>& c_value()
    {
        return m_v;
    }

    std::vector<T>& value()
    {
        return m_v;
    }

private:
    std::vector<T> m_v;
};

template <>
class _at<v8::Local<v8::Object>> {
public:
    _at(v8::Local<v8::Object>& v)
    {
        m_isolate = Isolate::current(v)->m_isolate;
        m_v.Reset(m_isolate, v);
    }

    v8::Local<v8::Object> c_value()
    {
        return v8::Local<v8::Object>();
    }

    v8::Local<v8::Object> value()
    {
        return m_v.Get(m_isolate);
    }

private:
    v8::Isolate* m_isolate;
    v8::Global<v8::Object> m_v;
};

class NType;
class AsyncCallBack : public AsyncEvent {
public:
    AsyncCallBack(v8::Local<v8::Object> cb, object_base* pThis = NULL);
    ~AsyncCallBack();

    virtual kCallType callType() const override
    {
        return kAsyncCallBack;
    }

public:
    virtual void resume()
    {
        async(m_v);
    }

public:
    int32_t post_result(int32_t v)
    {
        if (v == CALL_E_EXCEPTION) {
            m_error_code = Runtime::errCode();
            m_error_type = Runtime::errType();
            m_error = Runtime::errMessage();
        }

        m_v = v;
        m_isolate->sync([this]() -> int {
            return syncFunc();
        });

        return 0;
    }

    virtual int32_t post(int32_t v);

    virtual Isolate* isolate()
    {
        ex_assert(m_isolate);
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
        args.push_back(GetReturnValue(m_isolate, v));
    }

    virtual void to_args(std::vector<v8::Local<v8::Value>>& args)
    {
    }

    int syncFunc();

    void processPromiseResult();

protected:
    obj_ptr<object_base> m_pThis;
    v8::Global<v8::Object> m_cb;
    bool m_is_promise;
    obj_ptr<NType> m_result;

private:
    exlib::string m_error;
    result_t m_error_code = 0;
    ErrorType m_error_type = kError;
    v8::Global<v8::StackTrace> m_stack_trace;
    v8::Global<v8::Value> m_async_ctx;  // Captured async context for AsyncLocalStorage
    int32_t m_v;
};
}
