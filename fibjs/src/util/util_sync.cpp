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
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    int32_t len = args.Length();
    if (len > 0)
        _data->Set(context, NewString(isolate, "_error"), args[0]);

    if (len > 1)
        _data->Set(context, NewString(isolate, "_result"), args[1]);

    obj_ptr<Event_base> ev = Event_base::getInstance(JSValue(_data->Get(context, NewString(isolate, "_ev"))));
    ev->set();
}

static void sync_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    obj_ptr<Event_base> ev = new Event();
    v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);
    std::vector<v8::Local<v8::Value>> argv;

    _data->Set(context, isolate->NewString("_ev"), ev->wrap());

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
    str += isolate->toString(func->GetName());
    str += ")";

    METHOD_NAME(str.c_str());

    v8::Local<v8::Value> result;

    func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).ToLocal(&result);
    if (result.IsEmpty())
        return;

    ev->wait();

    JSValue error = _data->Get(context, isolate->NewString("_error"));

    if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
        isolate->m_isolate->ThrowException(error);
    else
        args.GetReturnValue().Set(JSValue(_data->Get(context, isolate->NewString("_result"))));
}

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    _data->Set(context, NewString(isolate, "_result"), args[0]);

    obj_ptr<Event_base> ev = Event_base::getInstance(JSValue(_data->Get(context, NewString(isolate, "_ev"))));
    ev->set();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    _data->Set(context, NewString(isolate, "_error"), args[0]);

    obj_ptr<Event_base> ev = Event_base::getInstance(JSValue(_data->Get(context, NewString(isolate, "_ev"))));
    ev->set();
}

static void promise_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    std::vector<v8::Local<v8::Value>> argv;

    int32_t len = args.Length();
    int32_t i;

    argv.resize(len);
    for (i = 0; i < len; i++)
        argv[i] = args[i];

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args.Data());
    v8::Local<v8::Value> result;
    func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).ToLocal(&result);
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

        v = o->Get(context, isolate->NewString("then"));
        if (v->IsFunction())
            _then = v8::Local<v8::Function>::Cast(v);

        v = o->Get(context, isolate->NewString("catch"));
        if (v->IsFunction())
            _catch = v8::Local<v8::Function>::Cast(v);
    }

    if (_promise.IsEmpty() && (_then.IsEmpty() || _catch.IsEmpty())) {
        ThrowError("not async function.");
        return;
    }

    obj_ptr<Event_base> ev = new Event();
    v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);
    _data->Set(context, isolate->NewString("_ev"), ev->wrap());

    v8::Local<v8::Function> _then_func;
    v8::Local<v8::Function> _catch_func;

    _then_func = isolate->NewFunction("promise_then", promise_then, _data);
    if (_then_func.IsEmpty()) {
        ThrowError("function alloc error.");
        return;
    }
    _catch_func = isolate->NewFunction("promise_catch", promise_catch, _data);
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

    ev->wait();

    JSValue error = _data->Get(context, isolate->NewString("_error"));

    if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
        isolate->m_isolate->ThrowException(error);
    else
        args.GetReturnValue().Set(JSValue(_data->Get(context, isolate->NewString("_result"))));
}

result_t util_base::sync(v8::Local<v8::Function> func, bool async_func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> func1;

    if (async_func || func->IsAsyncFunction()) {
        func1 = isolate->NewFunction("sync", promise_stub, func);
        if (func1.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));

        func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), func);
    } else {
        func1 = isolate->NewFunction("sync", sync_stub, func);
        if (func1.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));

        func1->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);
    }

    retVal = func1;

    return 0;
}
}
