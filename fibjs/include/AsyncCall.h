#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <functional>
#include <optional>
#include <tuple>
#include <utility>
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
        post(0);
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

public:
    // The union discipline (plans/idl-union-types-2026-10-02.md §3.15): a union
    // parameter that carries a v8 alternative is preprocessed in the sync phase
    // and carried in m_ctx; the async phase only reads the slot back, and asks
    // for it first. A missing slot means the sync phase never ran (the
    // compile-cache entry cc_<name> calls the implementation straight in the
    // async phase), which is a programming error of that entry, not something
    // to dereference.
    result_t ctx(size_t n)
    {
        if (m_ctx.size() <= n)
            return Runtime::setError(CALL_E_INVALIDARG,
                "the union parameter at ctx[%d] was not prepared: the sync phase of this entry did not run.", (int32_t)n);

        return 0;
    }

protected:
    void captureErrorContext(int32_t v)
    {
        // Success path: nothing to carry. The slot stays empty instead of
        // constructing an empty payload, and an event that never failed never
        // allocates one either.
        if (v >= 0) {
            m_errorPayload.reset();
            return;
        }

        ErrorPayload payload = takeErrorPayload();

        if (payload.empty()) {
            m_errorPayload.reset();
            return;
        }

        if (m_errorPayload)
            *m_errorPayload = std::move(payload);
        else
            m_errorPayload.reset(new ErrorPayload(std::move(payload)));
    }

    void applyErrorContext() const
    {
        if (m_errorPayload && !m_errorPayload->empty())
            setErrorPayload(*m_errorPayload);
    }

protected:
    Isolate* m_isolate;

    // Payloads only exist on failure: keeping them behind a pointer leaves every
    // successfully completed event free of payload construction, and of the
    // ~130 bytes the payload itself would otherwise occupy.
    static const ErrorPayload& emptyErrorPayload()
    {
        static const ErrorPayload s_empty;
        return s_empty;
    }

    const ErrorPayload& errorPayload() const
    {
        return m_errorPayload ? *m_errorPayload : emptyErrorPayload();
    }

private:
    kStateType m_state;
    std::unique_ptr<ErrorPayload> m_errorPayload;
};

// A single-owner handle to a continuation event, and the only way to name one
// (plans/async-handle-protocol-minimal-2026-10-08.md §2.3, §2.5).
//
// Root invariant: a continuation event has exactly one deleter at any time.
//   - owned    : the handle is that deleter. It deletes the event after a
//                delivery, and when an undelivered event is abandoned (handle
//                destroyed or overwritten). Delivery is always the event's own
//                post(), which runs the event's work on the spot and returns
//                only when that work is done - inline on the caller's stack,
//                or on a pool fiber when the caller hands the continuation to
//                the pool with AsyncHandle::apost. The event must put its work
//                in post() and must not dispose of itself.
//   - borrowed : the event's original owner deletes it (the glue stack frame,
//                a self-deleting machine or receiver). The handle never
//                deletes, and abandoning a borrowed continuation is a no-op.
//
// Delivery is detach -> deliver -> dispose: the handle empties itself *before*
// running the event, so re-entrant code (a downstream callback may destroy the
// object holding this handle) can never deliver or delete twice, and at no
// instant do the handle and the delivery path both hold the delete right.
//
// Migration interop is one-way, "in" only: AsyncHandle(AsyncEvent*) is an
// implicit *borrowed* construction, so unmigrated callers keep working
// unchanged (plans/async-handle-protocol-minimal-2026-10-08.md §2.5). There is
// deliberately no way back out to a raw pointer - a handle never surrenders
// the delete right it holds.
class AsyncHandle {
public:
    AsyncHandle() = default;

    // The "in" direction of the migration interop: a raw pointer becomes a
    // borrowed handle. The semantics are identical to today's raw protocol
    // (the event is delivered, never deleted by us), so unmigrated call sites
    // keep working unchanged. Not explicit on purpose - this is what makes
    // layer-by-layer migration possible.
    AsyncHandle(AsyncEvent* ac)
        : m_ac(ac)
        , m_borrowed(true)
    {
    }

    // owned: take over a heap one-shot event created for this call. The handle
    // becomes its unique deleter - after delivery, when abandoned before
    // delivery, and when overwritten. The event must put its work in post()
    // and must not dispose of itself (see the class comment).
    static AsyncHandle adopt(AsyncEvent* ev)
    {
        AsyncHandle h;
        h.m_ac = ev;
        h.m_borrowed = false;
        return h;
    }

    AsyncHandle(AsyncHandle&& other) noexcept
        : m_ac(other.m_ac)
        , m_borrowed(other.m_borrowed)
    {
        other.m_ac = nullptr;
        other.m_borrowed = true;
    }

    AsyncHandle& operator=(AsyncHandle&& other) noexcept
    {
        if (this != &other) {
            abandon();
            m_ac = other.m_ac;
            m_borrowed = other.m_borrowed;
            other.m_ac = nullptr;
            other.m_borrowed = true;
        }
        return *this;
    }

    AsyncHandle(const AsyncHandle&) = delete;
    AsyncHandle& operator=(const AsyncHandle&) = delete;

    ~AsyncHandle()
    {
        abandon();
    }

public:
    bool empty() const
    {
        return m_ac == nullptr;
    }
    explicit operator bool() const
    {
        return m_ac != nullptr;
    }

    bool isSync() const
    {
        ex_assert(m_ac);
        return m_ac->isSync();
    }
    bool isAsync() const
    {
        ex_assert(m_ac);
        return m_ac->isAsync();
    }

    // The caller kind of the event (JS glue / callback / cc_ entry). A query
    // like isSync/isAsync, not a way out to the pointer: the background-write
    // fast path needs it (UVStream.h, fs_zip.cpp).
    AsyncEvent::kCallType callType() const
    {
        ex_assert(m_ac);
        return m_ac->callType();
    }
    Isolate* isolate() const
    {
        ex_assert(m_ac);
        return m_ac->isolate();
    }

    result_t ctx(size_t n)
    {
        ex_assert(m_ac);
        return m_ac->ctx(n);
    }

    std::vector<Variant>& ctxv()
    {
        ex_assert(m_ac);
        return m_ac->m_ctx;
    }
    obj_ptr<object_base>& ctxo()
    {
        ex_assert(m_ac);
        return m_ac->m_ctxo;
    }
    const obj_ptr<object_base>& ctxo() const
    {
        ex_assert(m_ac);
        return m_ac->m_ctxo;
    }

public:
    // Delivery is detach -> deliver -> dispose:
    //   1. detach (empty the handle) *before* the call, so that re-entrant
    //      code - a downstream callback may destroy the object holding this
    //      handle - finds an empty handle and can never deliver or delete
    //      twice, and at no instant do the handle and the delivery path both
    //      hold the delete right;
    //   2. deliver: ev->post(hr) - the one delivery primitive, which runs the
    //      event's work here and returns only when it is done;
    //   3. dispose: owned events are deleted by the handle, borrowed ones by
    //      their original owner (the glue stack frame, the self-deleting
    //      machine or receiver).
    void post(result_t hr = 0)
    {
        AsyncEvent* ev = m_ac;
        bool owned = !m_borrowed;

        m_ac = nullptr;
        m_borrowed = true;

        if (!ev)
            return;

        ev->post(hr);

        if (owned)
            delete ev;
    }

    // Asynchronous delivery: hand this continuation to the pool selected by
    // `mode`, so a caller whose context forbids running the continuation
    // inline (V8 weak callback, uv/nghttp2 thread, deep recursion) can still
    // deliver. The task carries the handle - and with it the ownership and the
    // disposal duty - so nothing is released early: the post, and the owned
    // disposal that follows it, both run on the pool fiber. An empty handle is
    // a no-op, like post().
    void apost(result_t hr = 0, int32_t mode = CALL_E_NOSYNC)
    {
        if (empty())
            return;

        // The pool task, same shape as AsyncFunc (async()'s local class in
        // acPool.cpp): run the body, then dispose of itself. A handle instead
        // of a std::function, because a handle is move-only and std::function
        // requires a copyable target.
        class AsyncPost : public AsyncEvent {
        public:
            AsyncPost(AsyncHandle&& h, result_t hr)
                : m_h(std::move(h))
                , m_hr(hr)
            {
                // The error context is thread/fiber-local, so it must travel
                // with the task: capture it here, in the caller's context, and
                // restore it on the pool fiber - otherwise a failed delivery
                // across the hop would lose its description (message / type /
                // payload) and be reported as a bare code. Same cross-thread
                // rule as AsyncBackgroundWrite's emit path.
                if (hr < 0) {
                    m_desc = Runtime::captureErrorDescription(hr);
                    m_payload = takeErrorPayload();
                }
            }

            virtual void invoke() override
            {
                if (m_hr < 0)
                    Runtime::applyErrorDescription(m_desc, m_payload);

                m_h.post(m_hr);
                delete this;
            }

        private:
            AsyncHandle m_h;
            result_t m_hr;
            Runtime::ErrorDescription m_desc;
            ErrorPayload m_payload;
        };

        (new AsyncPost(std::move(*this), hr))->async(mode);
    }

private:
    // A non-empty owned handle dropped without delivery is an abandoned
    // continuation: recycling it is exactly the delete right the handle holds.
    // A borrowed handle owns nothing and leaves the event alone.
    void abandon()
    {
        if (m_ac && !m_borrowed)
            delete m_ac;

        m_ac = nullptr;
        m_borrowed = true;
    }

private:
    AsyncEvent* m_ac = nullptr;
    bool m_borrowed = true;
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
        captureErrorContext(v);
        if (v == CALL_E_EXCEPTION) {
            m_error_code = Runtime::errCode();
            m_error_type_name = Runtime::errTypeName();
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

        if (m_v < 0)
            applyErrorContext();
        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(errorPayload(), m_error_type_name.c_str(), m_error_code, m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void** args;

private:
    exlib::string m_error;
    result_t m_error_code = 0;
    exlib::string m_error_type_name;
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
        captureErrorContext(v);
        if (v == CALL_E_EXCEPTION) {
            m_error_code = Runtime::errCode();
            m_error_type_name = Runtime::errTypeName();
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
        if (m_v < 0)
            applyErrorContext();
        if (m_v == CALL_E_EXCEPTION)
            Runtime::setError(errorPayload(), m_error_type_name.c_str(), m_error_code, m_error);

        return m_v;
    }

protected:
    exlib::Event weak;
    void** args;

private:
    exlib::string m_error;
    result_t m_error_code = 0;
    exlib::string m_error_type_name;
    int32_t m_v;
};

class AsyncState : public AsyncEvent {
public:
    // legacy form: a raw upstream pointer becomes a *borrowed* handle - the
    // same "deliver, never delete" semantics the machines have always had, so
    // unmigrated call sites (including `AsyncState(NULL)`) compile unchanged.
    AsyncState(AsyncEvent* ac)
        : m_ac(ac)
        , m_bAsyncState(false)
        , m_state(NULL)
        , m_next(NULL)
    {
        setAsync();
    }

    // migrated form: move the caller's handle in; the owned/borrowed state
    // travels with it. The caller's handle is emptied, so the caller must
    // return CALL_E_PENDDING after handing the continuation over.
    AsyncState(AsyncHandle& ac)
        : m_ac(std::move(ac))
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

        // The ticket as a continuation handle (plans §2.3, §3.4.2): a
        // *borrowed* handle - what it names is the machine itself (release) or
        // its Continuation proxy (check build), and both own themselves, so
        // the handle never deletes. It materializes only when the ticket is
        // actually handed over or stored; the discarded form, the synchronous
        // continuation `next(send);`, never converts and never abandons
        // anything.
        operator AsyncHandle() const
        {
            return AsyncHandle(m_as);
        }

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
        captureErrorContext(v);
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
            if (hr < 0) {
                applyErrorContext();
                hr = error(hr);
            }

#if defined(FIBJS_ASYNC_STATE_CHECK)
            // error() 内可能再次发牌（同步跳转），执行下一个状态函数之前同样就地消费
            if (m_cont.expect.load(std::memory_order_relaxed) == m_epoch.load(std::memory_order_relaxed))
                ++m_epoch;
#endif

            if (hr < 0 || !m_next) {
                if (bAsyncState && m_ac)
                    m_ac.post(hr);

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
        // invoke() is the queued entry - the pool runs it, so this dispatch is
        // an async-phase one: nobody reads a return value and the completion
        // must post to the upstream. apost_() set this before queueing;
        // setting it here keeps that semantic once a machine is started with
        // async() instead. No-op today: apost_() is the only route to invoke()
        // and it already sets the flag.
        m_bAsyncState = true;
        post_(m_v);
    }

    // 直接异步派发（非虚）：把机器作为池任务启动。与 AsyncHandle::apost（把
    // 一个 continuation 交给池投递）不同，这里处理的是 self-owned 机器的启动；
    // 仅供静态类型为 AsyncState 的创建点使用（(new asyncXxx(...))->apost(0)）。
    void apost(int32_t v)
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
        captureErrorContext(v);

        async(CALL_E_NOSYNC);
    }

public:
    virtual Isolate* isolate()
    {
        return m_ac.isolate();
    }

public:
    virtual void resume()
    {
        apost(0);
    }

private:
    AsyncHandle m_ac;
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

        // 非虚：票的异步回投（仅检测模式；常规投递走 post()）。
        void apost(int32_t v);
        virtual int32_t post(int32_t v) override;

        // 票被当作 Task_base 唤醒时（exlib::Locker 等待链等，见 TLSSocket/http2
        // 读锁交棒）必须与宿主机器的 resume 语义一致：AsyncState::resume() 就是
        // apost(0)（池跳转）。继承 AsyncEvent::resume()（post(0)）会让完成链在同一
        // 根 fiber 栈上内联递归——大响应下每条 16KB TLS record 一层，128KB 栈耗尽
        // 即 SIGSEGV。普通构建挂等待链的是机器本体，本来就走 apost，这里只是对齐。
        virtual void resume() override
        {
            apost(0);
        }

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

// One captured union alternative: plain values are copied; a v8 handle is kept
// alive in a Global and surfaces as an empty handle when the callback resumes
// — the same contract as _at<v8::Local<Object>> above.
template <typename T>
class _at_variant_alt {
public:
    _at_variant_alt(T v)
        : m_v(v)
    {
    }

    T c_value()
    {
        return m_v;
    }

    T value()
    {
        return m_v;
    }

private:
    T m_v;
};

template <typename T>
class _at_variant_alt<v8::Local<T>> {
public:
    _at_variant_alt(v8::Local<T> v)
        : m_isolate(Isolate::current(v)->m_isolate)
    {
        m_v.Reset(m_isolate, v);
    }

    v8::Local<T> c_value()
    {
        return v8::Local<T>();
    }

    v8::Local<T> value()
    {
        return m_v.Get(m_isolate);
    }

private:
    v8::Isolate* m_isolate;
    v8::Global<T> m_v;
};

// A union parameter (IDL `A|B`, `std::variant` in the generated signatures)
// captured for the callback phase: the alternative index is preserved, the
// non-v8 alternatives carry their values and the v8 alternatives surface as
// empty handles when the callback resumes. The synchronous phase sees the
// original value (the async macros pass it to the implementation directly),
// so `value()` mirrors that contract.
template <typename... Ts>
class _at<std::variant<Ts...>> {
public:
    _at(std::variant<Ts...>& v)
        : m_index(v.index())
    {
        capture(v, std::index_sequence_for<Ts...>());
    }

    std::variant<Ts...> c_value()
    {
        return convert(true, std::index_sequence_for<Ts...>());
    }

    std::variant<Ts...> value()
    {
        return convert(false, std::index_sequence_for<Ts...>());
    }

private:
    template <std::size_t... Is>
    void capture(std::variant<Ts...>& v, std::index_sequence<Is...>)
    {
        bool done = false;

        (void)std::initializer_list<bool>{
            (done || v.index() != Is
                 ? false
                 : (done = true, std::get<Is>(m_cap).emplace(std::get<Is>(v)), true))...
        };
    }

    template <std::size_t... Is>
    std::variant<Ts...> convert(bool resume, std::index_sequence<Is...>)
    {
        std::variant<Ts...> out;
        bool done = false;

        (void)std::initializer_list<bool>{
            (done || m_index != Is
                 ? false
                 : (done = true,
                    out.template emplace<Is>(resume
                        ? std::get<Is>(m_cap).value().c_value()
                        : std::get<Is>(m_cap).value().value()),
                    true))...
        };

        return out;
    }

    std::size_t m_index;
    std::tuple<std::optional<_at_variant_alt<Ts>>...> m_cap;
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
        captureErrorContext(v);
        if (v == CALL_E_EXCEPTION) {
            m_error_code = Runtime::errCode();
            m_error_type_name = Runtime::errTypeName();
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
    exlib::string m_error_type_name;
    v8::Global<v8::StackTrace> m_stack_trace;
    v8::Global<v8::Value> m_async_ctx;  // Captured async context for AsyncLocalStorage
    int32_t m_v;
};
}
