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
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());
    v8::Local<v8::Value> argv[2] = {
        v8::Null(args.GetIsolate()), args[0]
    };

    func->Call(func->CreationContext(), args.This(), 2, argv);
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());
    v8::Local<v8::Value> argv[2] = {
        args[0], v8::Null(args.GetIsolate())
    };

    func->Call(func->CreationContext(), args.This(), 2, argv);
}

static void async_promise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len - 1);
    for (i = 0; i < len - 1; i++)
        argv[i] = args[i];

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());
    v8::Local<v8::Value> result;
    func->Call(func->CreationContext(), args.This(), (int32_t)argv.size(), argv.data()).ToLocal(&result);
    if (result.IsEmpty())
        return;

    v8::Local<v8::Promise> _promise;
    v8::Local<v8::Function> _then;
    v8::Local<v8::Function> _catch;

    if (result->IsPromise()) {
        _promise = v8::Local<v8::Promise>::Cast(result);
    } else if (result->IsObject()) {
        JSValue v;
        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(result);

        v = o->Get(isolate->NewString("then"));
        if (v->IsFunction())
            _then = v8::Local<v8::Function>::Cast(v);

        v = o->Get(isolate->NewString("catch"));
        if (v->IsFunction())
            _catch = v8::Local<v8::Function>::Cast(v);
    }

    if (_promise.IsEmpty() && (_then.IsEmpty() || _catch.IsEmpty())) {
        ThrowError("not async function.");
        return;
    }

    v8::Local<v8::Function> _then_func;
    v8::Local<v8::Function> _catch_func;

    _then_func = isolate->NewFunction("promise_then", promise_then, args[len - 1]);
    if (_then_func.IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }
    _catch_func = isolate->NewFunction("promise_catch", promise_catch, args[len - 1]);
    if (_catch_func.IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }

    if (!_promise.IsEmpty()) {
        _promise->Then(isolate->context(), _then_func);
        _promise->Catch(isolate->context(), _catch_func);
    } else {
        _then->Call(_then->CreationContext(), result, 1, (v8::Local<v8::Value>*)&_then_func);
        _catch->Call(_catch->CreationContext(), result, 1, (v8::Local<v8::Value>*)&_catch_func);
    }
}

result_t util_base::callbackify(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Function> func1;

    func1 = isolate->NewFunction("async_promise", async_promise, func);
    if (func.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    retVal = func1;

    return 0;
}
}
