/*
 * util_callbackify.cpp
 *
 *  Created on: Apr 11, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"

namespace fibjs {

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> func = args.Data().As<v8::Function>();
    v8::Local<v8::Value> argv[2] = {
        v8::Null(args.GetIsolate()), args[0]
    };

    func->Call(func->GetCreationContextChecked(), args.This(), 2, argv).IsEmpty();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> func = args.Data().As<v8::Function>();
    v8::Local<v8::Value> argv[2] = {
        args[0], v8::Null(args.GetIsolate())
    };

    func->Call(func->GetCreationContextChecked(), args.This(), 2, argv).IsEmpty();
}

static void async_promise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len - 1);
    for (i = 0; i < len - 1; i++)
        argv[i] = args[i];

    v8::Local<v8::Function> func = args.Data().As<v8::Function>();
    v8::Local<v8::Value> result = func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).FromMaybe(v8::Local<v8::Value>());
    if (result.IsEmpty())
        return;

    if (!result->IsPromise()) {
        args.GetReturnValue().Set(result);
        return;
    }

    v8::Local<v8::Promise> _promise = result.As<v8::Promise>();
    _promise->Then(isolate->context(),
        isolate->NewFunction("promise_then", promise_then, args[len - 1]),
        isolate->NewFunction("promise_catch", promise_catch, args[len - 1]));
}

result_t util_base::callbackify(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current(func);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> func1;
    v8::Local<v8::Value> v;

    v = func->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async"))).FromMaybe(v8::Local<v8::Value>());
    if (!IsEmpty(v)) {
        retVal = v.As<v8::Function>();
        return 0;
    }

    func1 = isolate->NewFunction("async_promise", async_promise, func);
    if (func1.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func1);
    func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), func);

    v = func->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync"))).FromMaybe(v8::Local<v8::Value>());
    if (!IsEmpty(v))
        func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), v);

    v8::Local<v8::Value> name = func->GetName();
    if (!name.IsEmpty())
        func1->SetName(name.As<v8::String>());

    retVal = func1;

    return 0;
}
}
