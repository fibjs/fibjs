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
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    _data->Set(context, NewString(isolate, "_result"), args[0]).IsJust();

    obj_ptr<Event_base> ev = Event_base::getInstance(_data);
    ev->set();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    _data->Set(context, NewString(isolate, "_error"), args[0]).IsJust();

    obj_ptr<Event_base> ev = Event_base::getInstance(_data);
    ev->set();
}

v8::Local<v8::Value> Isolate::await(v8::Local<v8::Value> promise)
{
    v8::Local<v8::Context> _context = context();

    v8::Local<v8::Promise> _promise;
    v8::Local<v8::Function> _then;
    v8::Local<v8::Function> _catch;

    if (promise->IsPromise()) {
        _promise = v8::Local<v8::Promise>::Cast(promise);
    } else if (promise->IsObject()) {
        JSValue v;
        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(promise);

        v = o->Get(_context, NewString("then"));
        if (v->IsFunction())
            _then = v8::Local<v8::Function>::Cast(v);

        v = o->Get(_context, NewString("catch"));
        if (v->IsFunction())
            _catch = v8::Local<v8::Function>::Cast(v);
    }

    v8::Local<v8::Value> result;

    if (_promise.IsEmpty() && (_then.IsEmpty() || _catch.IsEmpty())) {
        ThrowError("not async function.");
        return result;
    }

    obj_ptr<Event_base> ev = new Event();
    v8::Local<v8::Object> _data = ev->wrap(Event_base::class_info().CreateInstance(this));

    v8::Local<v8::Function> _handlers[2];

    _handlers[0] = NewFunction("promise_then", promise_then, _data);
    if (_handlers[0].IsEmpty()) {
        ThrowError("function alloc error.");
        return result;
    }
    _handlers[1] = NewFunction("promise_catch", promise_catch, _data);
    if (_handlers[1].IsEmpty()) {
        ThrowError("function alloc error.");
        return result;
    }

    if (!_promise.IsEmpty()) {
        v8::MaybeLocal<v8::Promise> r = _promise->Then(_context, _handlers[0], _handlers[1]);
        if (r.IsEmpty()) {
            ThrowError("promise error.");
            return result;
        }
    } else {
        v8::MaybeLocal<v8::Value> v;

        v = _then->Call(_context, promise, 2, (v8::Local<v8::Value>*)&_handlers[0]);
        if (v.IsEmpty()) {
            ThrowError("promise error.");
            return result;
        }

        v = _catch->Call(_context, promise, 1, (v8::Local<v8::Value>*)&_handlers[1]);
        if (v.IsEmpty()) {
            ThrowError("promise error.");
            return result;
        }
    }

    ev->wait();

    JSValue error = _data->Get(_context, NewString("_error"));
    if (!IsEmpty(error))
        m_isolate->ThrowException(error);
    else
        result = _data->Get(_context, NewString("_result")).FromMaybe(v8::Local<v8::Value>());

    return result;
}

}
