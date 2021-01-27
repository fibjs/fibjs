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
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    obj_ptr<Event_base> ev;
    ev = Event_base::getInstance(_data->Get(NewString(isolate, "_ev")));

    int32_t len = args.Length();

    if (len > 0)
        _data->Set(NewString(isolate, "_error"), args[0]);

    if (len > 1)
        _data->Set(NewString(isolate, "_result"), args[1]);

    ev->set();
}

static void sync_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<Event_base> ev = new Event();
    v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);
    std::vector<v8::Local<v8::Value>> argv;

    _data->Set(isolate->NewString("_ev"), ev->wrap());

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

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());

    exlib::string str("util.sync(");
    str += ToCString(v8::String::Utf8Value(isolate->m_isolate, func->GetName()));
    str += ")";

    METHOD_NAME(str.c_str());

    v8::Local<v8::Value> result = func->Call(args.This(), (int32_t)argv.size(), argv.data());
    if (result.IsEmpty())
        return;

    isolate->m_isolate->RunMicrotasks();
    ev->wait();

    JSValue error = _data->Get(isolate->NewString("_error"));

    if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
        isolate->m_isolate->ThrowException(error);
    else
        args.GetReturnValue().Set(_data->Get(isolate->NewString("_result")));
}

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());
    v8::Local<v8::Value> argv[2] = {
        v8::Null(args.GetIsolate()), args[0]
    };

    func->Call(args.This(), 2, argv);
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());
    v8::Local<v8::Value> argv[2] = {
        args[0], v8::Null(args.GetIsolate())
    };

    func->Call(args.This(), 2, argv);
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
    v8::Local<v8::Value> result = func->Call(args.This(), (int32_t)argv.size(), argv.data());
    if (result.IsEmpty())
        return;

    v8::Local<v8::Function> _then;
    v8::Local<v8::Function> _catch;
    JSValue v;

    if (result->IsObject()) {
        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(result);

        v = o->Get(isolate->NewString("then"));
        if (v->IsFunction())
            _then = v8::Local<v8::Function>::Cast(v);

        v = o->Get(isolate->NewString("catch"));
        if (v->IsFunction())
            _catch = v8::Local<v8::Function>::Cast(v);
    }

    if (_then.IsEmpty() || _catch.IsEmpty()) {
        ThrowError("not async function.");
        return;
    }

    v = isolate->NewFunction("promise_then", promise_then, args[len - 1]);
    if (v.IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }
    _then->Call(result, 1, &v);

    v = isolate->NewFunction("promise_catch", promise_catch, args[len - 1]);
    if (v.IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }
    _catch->Call(result, 1, &v);
}

result_t util_base::sync(v8::Local<v8::Function> func, bool async_func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Function> func1;

    if (async_func || func->IsAsyncFunction()) {
        func = isolate->NewFunction("async_promise", async_promise, func);
        if (func.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));
    }

    func1 = isolate->NewFunction("sync", sync_stub, func);
    if (func1.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    func1->SetPrivate(func1->CreationContext(),
        v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);

    retVal = func1;

    return 0;
}
}
