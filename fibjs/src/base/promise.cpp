/*
 * promise.cpp
 *
 *  Created on: Jun 14, 2023
 *      Author: lion
 */

#include "object.h"
#include "Event.h"

namespace fibjs {

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();

    _data->Set(context, NewString(isolate, "_result"), args[0]).IsJust();

    obj_ptr<Event_base> ev = Event_base::getInstance(_data);
    ev->set();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();

    _data->Set(context, NewString(isolate, "_error"), args[0]).IsJust();

    obj_ptr<Event_base> ev = Event_base::getInstance(_data);
    ev->set();
}

v8::Local<v8::Value> Isolate::await(v8::Local<v8::Value> promise)
{
    if (promise.IsEmpty() || !promise->IsPromise())
        return promise;

    v8::Local<v8::Context> _context = context();

    v8::Local<v8::Promise> _promise = promise.As<v8::Promise>();
    v8::Local<v8::Value> result;

    obj_ptr<Event_base> ev = new Event();
    v8::Local<v8::Object> _data = ev->wrap(Event_base::class_info().CreateInstance(this));

    _promise->Then(_context,
                NewFunction("promise_then", promise_then, _data),
                NewFunction("promise_catch", promise_catch, _data));

    ev->wait();

    JSValue error = _data->Get(_context, NewString("_error"));
    if (!IsEmpty(error))
        m_isolate->ThrowException(error);
    else
        result = _data->Get(_context, NewString("_result")).FromMaybe(v8::Local<v8::Value>());

    return result;
}

}
