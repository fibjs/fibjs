/*
 * Fiber.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "object.h"
#include "Fiber.h"
#include "ifs/os.h"
#include "ifs/process.h"
#include "options.h"
#include "EventEmitter.h"

namespace fibjs {

#define MAX_IDLE 256

int32_t g_spareFibers = MAX_IDLE;
static exlib::fiber_local<JSFiber*> s_current;

void JSFiber::FiberProcRunJavascript(void* p)
{
    result_t hr = 0;
    Isolate* isolate = (Isolate*)p;

    {
        Runtime rtForThread(isolate);
        v8::Locker locker(isolate->m_isolate);
        v8::Isolate::Scope isolate_scope(isolate->m_isolate);

        v8::HandleScope handle_scope(isolate->m_isolate);
        v8::Context::Scope context_scope(isolate->m_context.Get(isolate->m_isolate));

        isolate->m_idleFibers--;
        while (1) {
            if (!isolate->m_sem.trywait()) {
                isolate->m_idleFibers++;
                if (isolate->m_idleFibers > g_spareFibers) {
                    isolate->m_idleFibers--;
                    break;
                }

                {
                    v8::Unlocker unlocker(isolate->m_isolate);
                    isolate->m_sem.wait();
                }

                isolate->m_idleFibers--;
            }

            if (isolate->m_idleFibers == 0) {
                isolate->m_currentFibers++;
                isolate->m_idleFibers++;

                exlib::Service::CreateFiber(FiberProcRunJavascript, isolate, stack_size * 1024, "JSFiber");
            }

            {
                v8::HandleScope handle_scope(isolate->m_isolate);
                AsyncEvent* ae = (AsyncEvent*)isolate->m_urgentJobs.getHead();
                if (!ae)
                    ae = (AsyncEvent*)isolate->m_jobs.getHead();

                hr = ae->js_invoke();
            }

            isolate->Unref(hr);
        }

        isolate->m_currentFibers--;
    }

    isolate->m_isolate->DiscardThreadSpecificMetadata();
}

void JSFiber::start(bool urgent)
{
    Ref();
    auto func = [this]() -> int {
        return js_invoke();
    };

    if (urgent)
        holder()->sync_urgent(func);
    else
        holder()->sync(func);
}

result_t JSFiber::join()
{
    if (!m_quit.isSet()) {
        Isolate::LeaveJsScope _rt(holder());
        m_quit.wait();
        return _rt.is_terminating() ? CALL_E_TIMEOUT : 0;
    }

    return 0;
}

save_method_name::save_method_name(const char* name)
{
    m_fb = JSFiber::current();

    m_name = m_fb->m_native_name;
    m_fb->m_native_name = name;
}

save_method_name::~save_method_name()
{
    m_fb->m_native_name = m_name;
}

result_t JSFiber::get_id(int64_t& retVal)
{
    retVal = m_id;
    return 0;
}

result_t JSFiber::get_stack(exlib::string& retVal)
{
    if (JSFiber::current() == this)
        retVal = traceInfo(holder()->m_isolate, 300);
    else {
        exlib::string str;

        if (m_native_name) {
            str = "    at ";
            str += m_native_name;
            str += " (native code)";
        }

        if (m_c_entry_fp_ && m_handler_) {
            exlib::string str1 = traceInfo(holder()->m_isolate, 300, m_c_entry_fp_, m_handler_);
            if (!str1.empty()) {
                if (m_native_name)
                    str += '\n';
                str += str1;
            }
        }

        retVal = str;
    }

    return 0;
}

result_t JSFiber::get_stack_usage(int32_t& retVal)
{
    if (JSFiber::current() == this) {
        V8FrameInfo _fi = save_fi(holder()->m_isolate);
        if (_fi.entry_fp)
            retVal = (int32_t)(m_bind_thread->stack_start() - (intptr_t)_fi.entry_fp);
        else
            retVal = 0;
    } else if (m_bind_thread)
        retVal = (int32_t)(m_bind_thread->stack_start() - (intptr_t)m_c_entry_fp_);
    else
        retVal = 0;

    return 0;
}

JSFiber* JSFiber::current()
{
    return s_current;
}

// Handle unhandled promise rejections left when the outermost JS scope exits.
// Aligns with node >= 15: when a promise has no handler and process has no
// 'unhandledRejection' listener, log the error and terminate the process with
// exit code 1 (skipping 'beforeExit'). When a listener exists, dispatch the
// event with (reason, promise) instead and keep running.
static void handleUnhandledPromiseRejections(Isolate* isolate,
    std::vector<std::pair<v8::Global<v8::Value>, v8::Global<v8::Value>>>&& errors)
{
    // std::function requires a copyable callable, so share the move-only
    // error list through a shared_ptr.
    auto pErrors = std::make_shared<std::vector<std::pair<v8::Global<v8::Value>, v8::Global<v8::Value>>>>(std::move(errors));

    isolate->sync([isolate, pErrors]() -> int {
        JSFiber::EnterJsScope s;
        JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));

        int32_t count = 0;
        t.listenerCount("unhandledRejection", count);

        if (count > 0) {
            for (auto& e : *pErrors) {
                v8::Local<v8::Value> args[2] = {
                    e.second.Get(isolate->m_isolate),
                    e.first.Get(isolate->m_isolate)
                };
                bool r;
                t._emit("unhandledRejection", args, 2, r);
            }
            return 0;
        }

        for (auto& e : *pErrors) {
            v8::Local<v8::Value> reason = e.second.Get(isolate->m_isolate);
            if (reason->IsNativeError())
                errorLog(GetException(reason, false, true));
            else
                errorLog(isolate->toString(reason));
        }

        // Worker isolate crash semantics (propagating the error to the master)
        // belong to the deferred D2 work; here we keep the log-only behavior.
        if (!isolate->m_parent_worker)
            process_base::exit(1);

        return 0;
    });
}

result_t JSFiber::js_invoke()
{
    EnterJsScope s(this);
    v8::Local<v8::Value> retVal;

    size_t i;
    Isolate* isolate = holder();
    std::vector<v8::Local<v8::Value>> argv;
    v8::Local<v8::Function> func = m_func.Get(isolate->m_isolate);
    v8::Local<v8::Object> pThis = m_this.Get(isolate->m_isolate);

    argv.resize(m_argv.size());
    for (i = 0; i < m_argv.size(); i++)
        argv[i] = m_argv[i].Get(isolate->m_isolate);

    clear();

    retVal = func->Call(func->GetCreationContextChecked(), pThis, (int32_t)argv.size(), argv.data()).FromMaybe(v8::Local<v8::Value>());
    if (!IsEmpty(retVal))
        m_result.Reset(isolate->m_isolate, retVal);

    Unref();

    return 0;
}

JSFiber::EnterJsScope::EnterJsScope(JSFiber* fb)
    : m_hr(0)
    , m_pFiber(fb)
{
    if (fb == NULL)
        m_pFiber = new JSFiber();

    s_current = m_pFiber;
    m_pFiber->m_bind_thread = exlib::Thread_base::current();

    Isolate* isolate = m_pFiber->holder();

    isolate->m_fibers.putTail(m_pFiber);
    isolate->m_js_scope_depth++;

    m_fiber.Reset(isolate->m_isolate, m_pFiber->wrap(isolate));
}

JSFiber::EnterJsScope::~EnterJsScope()
{
    Isolate* isolate = m_pFiber->holder();

    isolate->RunMicrotasks(Isolate::MicrotaskCheckpointReason::kJsScopeLeave);

    m_pFiber->m_message = ReportException(try_catch, m_hr, false);

    isolate->m_js_scope_depth--;

    // Only handle unhandled promise rejections when the outermost JS scope exits,
    // giving inner scopes' callers a chance to attach .catch() handlers.
    if (isolate->m_js_scope_depth == 0 && !isolate->m_promise_error.IsEmpty()) {
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Array> _promise_error = isolate->m_promise_error.Get(isolate->m_isolate);
        JSArray ks = _promise_error->GetPropertyNames(_context);
        int32_t len = ks->Length();

        std::vector<std::pair<v8::Global<v8::Value>, v8::Global<v8::Value>>> errors;
        for (int32_t i = 0; i < len; i++) {
            JSValue v = _promise_error->Get(_context, JSValue(ks->Get(_context, i)));
            if (v->IsArray()) {
                v8::Local<v8::Array> o = v.As<v8::Array>();
                v8::Local<v8::Value> promise = o->Get(_context, 0).FromMaybe(v8::Local<v8::Value>());
                v8::Local<v8::Value> reason = o->Get(_context, 1).FromMaybe(v8::Local<v8::Value>());
                if (!reason.IsEmpty())
                    errors.emplace_back(
                        v8::Global<v8::Value>(isolate->m_isolate, promise),
                        v8::Global<v8::Value>(isolate->m_isolate, reason));
            }
        }

        isolate->m_promise_error.Reset();
        isolate->m_promise_error_no = 0;

        if (!errors.empty())
            handleUnhandledPromiseRejections(isolate, std::move(errors));
    }

    m_pFiber->m_quit.set();

    m_pFiber->holder()->m_fibers.remove(m_pFiber);
    s_current = 0;
}

} /* namespace fibjs */
