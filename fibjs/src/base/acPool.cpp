/*
 * date.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: lion
 */

#include "object.h"
#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include <unordered_map>
#include "Fiber.h"

namespace fibjs {

#define WORKER_STACK_SIZE 128

// A pool worker used to retire as soon as more than m_max_idle workers were
// idle, so bursty load (an HTTP server under keep-alive traffic, for example)
// malloc'ed and freed a WORKER_STACK_SIZE stack for almost every request:
// measured with `strace -e trace=mmap`, a 32-connection load produced ~27k
// mmap/munmap pairs of 132KB in 2s (~1 per request).  128KB is exactly at
// glibc's default mmap threshold, so that churn also made the process depend
// on glibc's dynamic threshold adjustment (disabling trim, for instance,
// freezes the threshold and turns every request into an mmap -11% throughput).
//
// Park grown workers instead of destroying their fibers, up to this cap.
// Parked fibers only hold the 128KB stack reservation (pages are committed
// lazily), and the worker loop below is the only place that retires them, so
// the pool keeps its stacks in the steady state.
#define MAX_POOL_WORKERS 1024

// Idle workers of the blocking pool retire after this long without work, and
// only while the pool is above its floor: an OS thread costs ~20us to create
// (measured with a pthread_create+join loop: 19-26us on macOS) while parking
// one is almost free, so the floor is kept ready for the next burst.
#define POOL_IDLE_MS 1000

// ---------------------------------------------------------------------------
// Fiber pool (result_t::nosync): the work is not blocking, it only has to run
// later on the isolate thread.  Workers are fibers of that one thread, so they
// cooperate: the pool is a queue plus parked fibers, with no need to know how
// many of them are busy.
//
// Growth/retirement is the historical policy described above: park grown
// workers up to MAX_POOL_WORKERS and retire only over the cap, so a bursty
// server does not malloc/free a stack per request.
// ---------------------------------------------------------------------------
class FiberPool {
public:
    FiberPool(int32_t max_idle)
        : m_max_idle(max_idle)
        , m_idleWorkers(1)
    {
        new_worker();
    }

public:
    void put(AsyncEvent* ac)
    {
        m_pool.put(ac);
    }

private:
    void new_worker()
    {
        if (m_workers.inc() > MAX_POOL_WORKERS) {
            m_workers.dec();
            return;
        }

        // The worker itself accounts for being idle (m_idleWorkers.dec() at
        // entry); decrementing here as well would leave the counter negative
        // and the pool would then never grow (its pickup path only spawns when
        // the counter lands exactly on 0).
        exlib::Service::CreateFiber(FiberProcWorker, this, WORKER_STACK_SIZE * 1024, "WorkerFiber");
    }

    void FiberProcWorker()
    {
        Runtime rtForThread(NULL);
        AsyncEvent* p;

        m_idleWorkers.dec();

        while (true) {
            if (m_idleWorkers.inc() > m_max_idle) {
                // Too many idle workers: park this one in m_pool.get() below
                // instead of retiring it, so its fiber stack is reused (see
                // MAX_POOL_WORKERS); only retire beyond the cap.
                bool bRetire = false;

                if (m_idleWorkers.dec() > 0) {
                    bool bOverCap = m_workers.dec() >= MAX_POOL_WORKERS;

                    if (bOverCap)
                        bRetire = true;
                    else
                        m_workers.inc();
                }

                if (bRetire)
                    break;

                m_idleWorkers.inc();
            }

            p = m_pool.get();
            if (m_idleWorkers.dec() == 0)
                if (m_idleWorkers.CompareAndSwap(0, 1) == 0)
                    new_worker();

            p->invoke();
        }
    }

    static void FiberProcWorker(void* ptr)
    {
        ((FiberPool*)ptr)->FiberProcWorker();
    }

private:
    int32_t m_max_idle;
    exlib::Queue<AsyncEvent> m_pool;
    exlib::atomic m_idleWorkers;
    exlib::atomic m_workers;
};

// ---------------------------------------------------------------------------
// Thread pool (result_t::longsync): real blocking work, one OS thread per task
// in flight.  Every synchronous call that has to block (db, dns, zlib, crypto,
// vector scans) hops through here, so the policy is tuned for latency:
//
//   * a floor of workers stays parked (a parked thread costs no memory),
//     workers above the floor retire only after POOL_IDLE_MS of silence;
//   * put() spawns a worker itself when queued tasks outnumber idle workers,
//     so a burst creates its threads in parallel instead of one per task
//     pickup.
//
// A worker-side spin before parking was tried and dropped: it never removed the
// caller's wake-up (the expensive half of a pool hop) and competed with the
// woken JS thread for a core. Measurements in
// plans/acpool-split-analysis-2026-11-26.md.
//
// Measured effect (8 logical CPUs, macOS): 7 way fan-out went from 544us to
// 442us, a 100 row vector query from 28.0us to 25.0us; the single task hop
// (~9us, two cross-thread wake-ups) is unchanged, and both spin variants tried
// for it made things worse.
// ---------------------------------------------------------------------------
class ThreadPool {
public:
    ThreadPool(int32_t floor)
        : m_floor(floor)
        , m_idle(floor)
    {
        for (int32_t i = 0; i < floor; i++) {
            m_workers.inc();
            create_worker();
        }
    }

public:
    void put(AsyncEvent* ac)
    {
        // Counter shared with the queue: lets put() decide whether a worker is
        // missing without locking the list.
        m_count.inc();
        m_pool.put(ac);

        // Nobody is waiting: start a worker now, in parallel with the task
        // being picked up, instead of waiting for a busy worker to finish.
        // Comparing against the queue length (and not just "no idle worker")
        // matters for bursts: tasks are queued faster than workers pick them
        // up, so the first one to notice idle > 0 must not swallow the rest.
        if (m_count.value() > m_idle.value())
            spawn();
    }

private:
    void spawn()
    {
        if (m_workers.inc() > MAX_POOL_WORKERS) {
            m_workers.dec();
            return;
        }

        m_idle.inc();
        create_worker();
    }

    void create_worker()
    {
        class _thread : public exlib::OSThread {
        public:
            typedef void (*thread_func)(void*);

        public:
            _thread(thread_func proc, void* arg)
                : m_proc(proc)
                , m_arg(arg)
            {
            }

        public:
            virtual void Run()
            {
#ifdef _WIN32
                CoInitializeEx(NULL, COINIT_MULTITHREADED);
                m_proc(m_arg);
                CoUninitialize();
#else
                m_proc(m_arg);
#endif
            }

        private:
            thread_func m_proc;
            void* m_arg;
        };

        (new _thread(worker_entry, this))->start();
    }

    static void worker_entry(void* ptr)
    {
        ((ThreadPool*)ptr)->worker();
    }

    AsyncEvent* take()
    {
        AsyncEvent* p = m_pool.tryget();

        if (p)
            m_count.dec();

        return p;
    }

    void worker()
    {
        Runtime rtForThread(NULL);

        // Invariant: a worker is counted as idle unless it is running a task.
        for (;;) {
            AsyncEvent* p = nullptr;

            if (m_count.value() > 0)
                p = take();

            if (!p) {
                bool above_floor = m_workers.value() > m_floor;

                if (m_pool.m_sem.wait(above_floor ? POOL_IDLE_MS : -1)) {
                    p = m_pool.m_list.getHead();
                    m_count.dec();
                } else {
                    // Timed out above the floor: retire, unless that would
                    // drop the pool below its floor (a concurrent worker may
                    // have retired first).
                    m_idle.dec();

                    if (m_workers.dec() >= m_floor)
                        return;

                    m_workers.inc();
                    m_idle.inc();
                    continue;
                }
            }

            m_idle.dec();
            p->invoke();
            m_idle.inc();
        }
    }

private:
    int32_t m_floor;
    exlib::Queue<AsyncEvent> m_pool;
    exlib::atomic m_count;   // queued tasks
    exlib::atomic m_idle;    // workers able to take a task
    exlib::atomic m_workers; // live workers
};

void putGuiPool(AsyncEvent* ac);

static FiberPool* s_acPool;
static ThreadPool* s_lsPool;

AsyncEvent::AsyncEvent(Isolate* isolate)
    : m_isolate(isolate)
    , m_state(kStateSync)
{
}

AsyncEvent::~AsyncEvent()
{
}

void AsyncEvent::async(result_t type)
{
    if (type.is_nosync())
        s_acPool->put(this);
    else if (type.is(CALL_E_LONGSYNC))
        s_lsPool->put(this);
    else if (type.is(CALL_E_GUILOOP))
        putGuiPool(this);
}

AsyncCallBack::AsyncCallBack(v8::Local<v8::Object> cb, object_base* pThis)
{
    if (pThis) {
        m_pThis = pThis;
        m_isolate = pThis->holder();
    } else
        m_isolate = Isolate::current(cb);

    m_isolate->Ref();
    m_cb.Reset(m_isolate->m_isolate, cb);
    m_is_promise = !cb->IsFunction();

    // Capture current stack trace for error reporting
    v8::Local<v8::StackTrace> stack = v8::StackTrace::CurrentStackTrace(
        m_isolate->m_isolate, 10, v8::StackTrace::kDetailed);
    m_stack_trace.Reset(m_isolate->m_isolate, stack);

    // Capture current async context for AsyncLocalStorage propagation
    JSFiber* fb = JSFiber::current();
    if (fb && !fb->m_async_ctx.IsEmpty())
        m_async_ctx.Reset(m_isolate->m_isolate, fb->m_async_ctx.Get(m_isolate->m_isolate));
}

AsyncCallBack::~AsyncCallBack()
{
    m_isolate->Unref();
    m_cb.Reset();
    m_stack_trace.Reset();
    m_async_ctx.Reset();
}

result_t AsyncCallBack::post(result_t v)
{
    if (m_pThis)
        m_pThis->leave(this);
    ex_assert(m_isolate);
    return post_result(v);
}

void AsyncCallBack::fillRetVal(std::vector<v8::Local<v8::Value>>& args, object_base* obj)
{
    v8::Local<v8::Value> v;
    obj->valueOf(v);
    args.push_back(v);
}

void AsyncCallBack::fillRetVal(std::vector<v8::Local<v8::Value>>& args, NType* v)
{
    m_result = v;
    if (!m_is_promise)
        v->to_args(m_isolate, args);
}

// Promises returned by async methods get a unified Symbol.asyncIterator
// (wrapper function cached per Isolate). The wrapper takes the promise as
// `this` and returns an async iterator whose next/return first await the
// promise to get the real result, then take its asyncIterator (the iterator
// follows the native async protocol, avoiding the Async-from-Sync infinite
// loop on next() returning a Promise).
// Effect: for await (var row of connP.iterate(...)) works directly.
static v8::Local<v8::Function> get_async_iterable_fn(Isolate* isolate)
{
    if (isolate->m_asyncIterFn.IsEmpty()) {
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::String> source = isolate->NewString(
            "(function () {"
            "    var self = this;"
            "    return {"
            "        next: function (v) {"
            "            return self.then(function (it) {"
            "                var iter = (it && it[Symbol.asyncIterator]) ? it[Symbol.asyncIterator]() : it;"
            "                return iter.next(v);"
            "            });"
            "        },"
            "        return: function (v) {"
            "            return self.then(function (it) {"
            "                var iter = (it && it[Symbol.asyncIterator]) ? it[Symbol.asyncIterator]() : it;"
            "                return (iter && iter.return) ? iter.return(v) : { done: true };"
            "            });"
            "        }"
            "    };"
            "})");
        v8::Local<v8::Script> script;
        if (!v8::Script::Compile(context, source).ToLocal(&script))
            return v8::Local<v8::Function>();

        v8::Local<v8::Value> fn;
        if (!script->Run(context).ToLocal(&fn) || fn.IsEmpty() || !fn->IsFunction())
            return v8::Local<v8::Function>();

        isolate->m_asyncIterFn.Reset(isolate->m_isolate, fn.As<v8::Function>());
    }

    return isolate->m_asyncIterFn.Get(isolate->m_isolate);
}

void AsyncCallBack::processPromiseResult()
{
    v8::Local<v8::Promise::Resolver> resolver = m_cb.Get(m_isolate->m_isolate).As<v8::Promise::Resolver>();

    if (m_v.is_return_null())
        resolver->Resolve(m_isolate->context(), v8::Null(m_isolate->m_isolate)).IsJust();
    else if (m_v.is_return_undefined())
        resolver->Resolve(m_isolate->context(), v8::Undefined(m_isolate->m_isolate)).IsJust();
    else if (m_v.ok()) {
        std::vector<v8::Local<v8::Value>> args;

        {
            v8::TryCatch try_catch(m_isolate->m_isolate);
            to_args(args);
            if (try_catch.HasCaught()) {
                resolver->Reject(m_isolate->context(), try_catch.Exception()).IsJust();
                delete this;
                return;
            }
        }

        v8::Local<v8::Value> result;
        if (m_result)
            m_result->valueOf(result);
        else if (args.size() == 0)
            result = v8::Undefined(m_isolate->m_isolate);
        else {
            result = args[0];

            if (result->IsObject()) {
                v8::Local<v8::Object> o = result.As<v8::Object>();
                obj_ptr<object_base> obj = object_base::getInstance(o);
                if (obj) {
                    ClassInfo& ci = obj->Classinfo();
                    if (ci.hasAsync())
                        o->SetPrototype(m_isolate->context(), ci.GetAsyncPrototype(m_isolate)).IsJust();
                }
            }
        }

        resolver->Resolve(m_isolate->context(), result).IsJust();
    } else {
        // The failing value carries its own description.
        v8::Local<v8::StackTrace> stack = m_stack_trace.Get(m_isolate->m_isolate);
        resolver->Reject(m_isolate->context(), FillError(m_v, stack)).IsJust();
    }

    delete this;
}

result_t AsyncCallBack::syncFunc()
{
    JSFiber::EnterJsScope s;

    // Restore async context for AsyncLocalStorage propagation
    JSFiber* fb = JSFiber::current();
    v8::Global<v8::Value> savedCtx;
    v8::Isolate* v8_isolate = m_isolate->m_isolate;
    if (fb) {
        if (!fb->m_async_ctx.IsEmpty())
            savedCtx.Reset(v8_isolate, fb->m_async_ctx.Get(v8_isolate));
        if (!m_async_ctx.IsEmpty())
            fb->m_async_ctx.Reset(v8_isolate, m_async_ctx.Get(v8_isolate));
        else
            fb->m_async_ctx.Reset();
    }

    if (m_is_promise) {
        processPromiseResult();
    } else {
        std::vector<v8::Local<v8::Value>> args;
        v8::Local<v8::Function> func = m_cb.Get(m_isolate->m_isolate).As<v8::Function>();

        if (m_v.is_return_null()) {
            args.resize(2);
            args[0] = v8::Null(m_isolate->m_isolate);
            args[1] = v8::Null(m_isolate->m_isolate);
        } else if (m_v.ok()) {
            args.resize(1);
            args[0] = v8::Null(m_isolate->m_isolate);
            to_args(args);
        } else {
            args.resize(1);
            v8::Local<v8::StackTrace> stack = m_stack_trace.Get(m_isolate->m_isolate);
            args[0] = FillError(m_v, stack);
        }

        v8::Local<v8::Value> oThis;
        if (m_result != nullptr)
            oThis = m_result->wrap(m_isolate);
        else
            oThis = v8::Undefined(m_isolate->m_isolate);

        func->Call(func->GetCreationContextChecked(), oThis, (int32_t)args.size(), args.data())
            .IsEmpty();

        delete this;
    }

    // Restore previous async context
    if (fb) {
        if (!savedCtx.IsEmpty())
            fb->m_async_ctx.Reset(v8_isolate, savedCtx.Get(v8_isolate));
        else
            fb->m_async_ctx.Reset();
    }

    return result_t();
}

result_t AsyncCallBack::check_result(result_t hr, const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (m_is_promise) {
        v8::Local<v8::Promise::Resolver> resolver = m_cb.Get(m_isolate->m_isolate).As<v8::Promise::Resolver>();
        v8::Local<v8::Promise> promise = resolver->GetPromise();
        args.GetReturnValue().Set(promise);

        // Install Symbol.asyncIterator on the Promise (adaptive wrapper, see
        // get_async_iterable_fn): for await (var row of connP.iterate(...))
        // can consume the return value of async methods directly
        //
        // Terminate 竞态防护：worker.terminate() 的硬中断（TerminateExecution）后，
        // V8 处于终止态，首次构造该 wrapper 时的 Script::Compile/Run 会失败并返回
        // 空句柄；把空句柄交给 Object::Set 会走到 V8 内部解引用空 handle 而崩溃
        // （实测栈：Object::Set → TransitionAndWriteDataProperty →
        // Map::TransitionToDataProperty → Handle<Object>::operator*，
        // 与 Buffer::load_module 已修的崩溃同源）。终止态下 worker 本就不该再执行 JS，
        // 跳过安装即可；后续 resolve/reject 路径保持原样。
        if (!m_isolate->is_terminating() && !m_isolate->m_isolate->IsExecutionTerminating()) {
            v8::Local<v8::Symbol> async_iter_symbol = v8::Symbol::GetAsyncIterator(m_isolate->m_isolate);
            v8::Local<v8::Function> async_iter_fn = get_async_iterable_fn(m_isolate);
            if (!async_iter_symbol.IsEmpty() && !async_iter_fn.IsEmpty())
                promise->Set(m_isolate->context(), async_iter_symbol, async_iter_fn).IsJust();
        }

        if (!hr.is_nosync() && !hr.is(CALL_E_LONGSYNC) && !hr.is(CALL_E_GUILOOP)) {
            m_v = hr;
            processPromiseResult();

            return result_t::return_undefined();
        }
    } else {
        if (m_ctxo)
            args.GetReturnValue().Set(GetReturnValue(m_isolate, m_ctxo));

        if (!hr.is_nosync() && !hr.is(CALL_E_LONGSYNC) && !hr.is(CALL_E_GUILOOP)) {
            post_result(hr);
            return result_t::return_undefined();
        }
    }

    m_v = hr;
    if (m_pThis == NULL || m_pThis->enterTask(this)) {
        if (hr.is_nosync()) {
            Isolate::LeaveJsScope _rt(m_isolate);
            invoke();

            if (_rt.is_terminating())
                m_v = result_t::timeout();
        } else {
            async(hr);
        }
    }

    return result_t::return_undefined();
}

void InitializeAcPool()
{
    // The blocking pool keeps a floor of parked threads: a thread costs ~20us
    // to create, so re-creating the workers of every burst is the expensive
    // part, and parking one costs no memory (measured: same RSS with 2 and 8
    // parked threads).  One per CPU, capped at 8, keeps both small machines
    // and big ones happy.
    int32_t cpus = 1;
    os_base::cpuNumbers(cpus);

    int32_t floor = cpus;

    if (floor < 2)
        floor = 2;
    if (floor > 8)
        floor = 8;

    s_acPool = new FiberPool(2);
    s_lsPool = new ThreadPool(floor);
}

void async(std::function<void(void)> func, result_t mode)
{
    class AsyncFunc : public AsyncEvent {
    public:
        AsyncFunc(std::function<void(void)> func)
            : m_func(func)
        {
        }

        virtual void invoke()
        {
            m_func();
            delete this;
        }

    private:
        std::function<void(void)> m_func;
    };

    (new AsyncFunc(func))->async(mode);
}

}
