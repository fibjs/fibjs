/*
 * util_sync.cpp
 *
 *  Created on: Jun 1, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "Event.h"

namespace fibjs {

static bool util_sync_should_stop(Isolate* isolate)
{
    return !isolate || isolate->is_terminating() || isolate->m_isolate->IsExecutionTerminating();
}

static void sync_callback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    Isolate* fib_isolate = Isolate::current(isolate);
    if (util_sync_should_stop(fib_isolate))
        return;

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Value> data = args.Data();
    if (data.IsEmpty())
        return;

    v8::Local<v8::Object> _data = data.As<v8::Object>();
    if (_data.IsEmpty())
        return;

    int32_t len = args.Length();
    if (len > 0)
        _data->Set(context, NewString(isolate, "_error"), args[0]).IsJust();

    if (len > 1)
        _data->Set(context, NewString(isolate, "_result"), args[1]).IsJust();

    obj_ptr<Event_base> ev = Event_base::getInstance(_data);
    ev->set();
}

static void sync_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    if (util_sync_should_stop(isolate))
        return;

    v8::Local<v8::Context> context = isolate->context();
    obj_ptr<Event_base> ev = new Event();
    v8::Local<v8::Object> _data = ev->wrap();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len + 1);
    for (i = 0; i < len; i++)
        argv[i] = args[i];

    argv[i] = isolate->NewFunction("sync_callback", sync_callback, _data);
    if (argv[i].IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }

    v8::Local<v8::Value> data = args.Data();
    if (data.IsEmpty())
        return;

    v8::Local<v8::Function> func = data.As<v8::Function>();
    if (func.IsEmpty())
        return;

    v8::Local<v8::Value> result = func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).FromMaybe(v8::Local<v8::Value>());
    if (result.IsEmpty())
        return;

    METHOD_NAME("util.sync.stub");

    // 登记到 isolate：终止时必须被唤醒。fiber park 在 exlib::Event::wait() 上，
    // 而 isolate 终止后 sync_callback 不会再 set()（V8 已终止时它甚至根本不会被
    // 调用），没有唤醒源就会永久挂住 → worker 退不出去、terminate() 永不收敛。
    isolate->addSyncWaiter(ev);
    ev->ac_wait();
    isolate->removeSyncWaiter(ev);

    JSValue error = _data->Get(context, isolate->NewString("_error"));

    if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
        isolate->m_isolate->ThrowException(error);
    else
        args.GetReturnValue().Set(JSValue(_data->Get(context, isolate->NewString("_result"))));
}

static void promise_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    if (util_sync_should_stop(isolate))
        return;

    v8::Local<v8::Context> context = isolate->context();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len);
    for (i = 0; i < len; i++)
        argv[i] = args[i];

    v8::Local<v8::Value> data = args.Data();
    if (data.IsEmpty())
        return;

    JSFunction func = data.As<v8::Function>();
    v8::Local<v8::Value> result;
    result = func.Call(args.This(), (int32_t)argv.size(), argv.data());
    if (result.IsEmpty())
        return;

    args.GetReturnValue().Set(result);
}

result_t util_base::sync(v8::Local<v8::Function> func, bool async_func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current(func);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> func1;
    v8::Local<v8::Value> v;

    v = func->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync"))).FromMaybe(v8::Local<v8::Value>());
    if (!IsEmpty(v)) {
        retVal = v.As<v8::Function>();
        return 0;
    }

    if (async_func || func->IsAsyncFunction()) {
        func1 = isolate->NewFunction("sync", promise_stub, func);
        if (func1.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));
        v = func->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async"))).FromMaybe(v8::Local<v8::Value>());

        func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), func);
    } else {
        func1 = isolate->NewFunction("sync", sync_stub, func);
        if (func1.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));
        v = func->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise"))).FromMaybe(v8::Local<v8::Value>());

        func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);
    }
    func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), func1);
    if (!IsEmpty(v))
        v.As<v8::Function>()->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), func1);

    v8::Local<v8::Value> name = func->GetName();
    if (!name.IsEmpty())
        func1->SetName(name.As<v8::String>());

    retVal = func1;

    return 0;
}
}
