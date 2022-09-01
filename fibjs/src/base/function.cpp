/*
 * function.cpp
 *
 *  Created on: Aug 9, 2021
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

v8::Local<v8::Value> JSFunction::Call(v8::Local<v8::Context> context, v8::Local<v8::Value> recv,
    int argc, v8::Local<v8::Value> argv[], bool async)
{
    v8::Local<v8::Value> result;
    (*this)->Call(context, recv, argc, argv).ToLocal(&result);
    if (result.IsEmpty())
        return result;

    if (async || (*this)->IsAsyncFunction()) {
        Isolate* isolate = Isolate::current();

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
            return result;
        }

        obj_ptr<Event_base> ev = new Event();
        v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);
        _data->Set(context, isolate->NewString("_ev"), ev->wrap());

        v8::Local<v8::Function> _handlers[2];

        _handlers[0] = isolate->NewFunction("promise_then", promise_then, _data);
        if (_handlers[0].IsEmpty()) {
            ThrowError("function alloc error.");
            return result;
        }
        _handlers[1] = isolate->NewFunction("promise_catch", promise_catch, _data);
        if (_handlers[1].IsEmpty()) {
            ThrowError("function alloc error.");
            return result;
        }

        if (!_promise.IsEmpty()) {
            v8::MaybeLocal<v8::Promise> r = _promise->Then(context, _handlers[0], _handlers[1]);
            if (r.IsEmpty()) {
                ThrowError("promise error.");
                return result;
            }
        } else {
            v8::MaybeLocal<v8::Value> v;

            v = _then->Call(_then->CreationContext(), result, 2, (v8::Local<v8::Value>*)&_handlers[0]);
            if (v.IsEmpty()) {
                ThrowError("promise error.");
                return result;
            }

            v = _catch->Call(_catch->CreationContext(), result, 1, (v8::Local<v8::Value>*)&_handlers[1]);
            if (v.IsEmpty()) {
                ThrowError("promise error.");
                return result;
            }
        }

        ev->wait();

        JSValue error = _data->Get(context, isolate->NewString("_error"));
        if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull())
            isolate->m_isolate->ThrowException(error);
        else
            _data->Get(context, isolate->NewString("_result")).ToLocal(&result);
    }

    return result;
}
}