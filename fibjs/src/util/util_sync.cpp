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

static void sync_callback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();

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

    v8::Local<v8::Function> func = args.Data().As<v8::Function>();
    v8::Local<v8::Value> result = func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).FromMaybe(v8::Local<v8::Value>());
    if (result.IsEmpty())
        return;

    ev->wait();

    JSValue error = _data->Get(context, isolate->NewString("_error"));

    if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
        isolate->m_isolate->ThrowException(error);
    else
        args.GetReturnValue().Set(JSValue(_data->Get(context, isolate->NewString("_result"))));
}

static void promise_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len);
    for (i = 0; i < len; i++)
        argv[i] = args[i];

    JSFunction func = args.Data().As<v8::Function>();
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
