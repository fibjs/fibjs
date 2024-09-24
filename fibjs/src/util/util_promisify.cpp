/*
 * util_promisify.cpp
 *
 *  Created on: Apr 1, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"

namespace fibjs {

static void promisify_callback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();

    v8::Local<v8::Promise::Resolver> resolver = _data.As<v8::Promise::Resolver>();

    int32_t len = args.Length();

    if (len > 0) {
        if (!args[0].IsEmpty() && !args[0]->IsUndefined() && !args[0]->IsNull()) {
            resolver->Reject(isolate->context(), args[0]).IsJust();
            return;
        }
    }

    if (len > 1) {
        resolver->Resolve(isolate->context(), args[1]).IsJust();
    } else
        resolver->Resolve(isolate->context(), v8::Undefined(isolate->m_isolate)).IsJust();
}

static void promisify_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len + 1);
    for (i = 0; i < len; i++)
        argv[i] = args[i];

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(isolate->context()).FromMaybe(v8::Local<v8::Promise::Resolver>());

    argv[i] = isolate->NewFunction("promisify_callback", promisify_callback, resolver);
    if (argv[i].IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }

    v8::Local<v8::Function> func = args.Data().As<v8::Function>();
    v8::Local<v8::Value> result = func->Call(func->GetCreationContextChecked(), args.This(), (int32_t)argv.size(), argv.data()).FromMaybe(v8::Local<v8::Value>());
    if (result.IsEmpty())
        return;

    args.GetReturnValue().Set(resolver->GetPromise());
}

result_t util_base::promisify(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current(func);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> func1;
    v8::Local<v8::Value> v;

    v = func->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise"))).FromMaybe(v8::Local<v8::Value>());
    if (!IsEmpty(v)) {
        retVal = v.As<v8::Function>();
        return 0;
    }

    func1 = isolate->NewFunction("promisify", promisify_stub, func);
    if (func1.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));
    setAsyncFunctoin(func1);

    func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), func1);
    func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);

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