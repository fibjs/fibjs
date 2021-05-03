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
    str += isolate->toString(func->GetName());
    str += ")";

    METHOD_NAME(str.c_str());

    v8::Local<v8::Value> result = func->Call(args.This(), (int32_t)argv.size(), argv.data());
    if (result.IsEmpty())
        return;

    ev->wait();

    JSValue error = _data->Get(isolate->NewString("_error"));

    if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
        isolate->m_isolate->ThrowException(error);
    else
        args.GetReturnValue().Set(_data->Get(isolate->NewString("_result")));
}

result_t util_base::sync(v8::Local<v8::Function> func, bool async_func, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Function> func1;

    if (async_func || func->IsAsyncFunction()) {
        result_t hr = callbackify(func, func);
        if (hr < 0)
            return hr;
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
