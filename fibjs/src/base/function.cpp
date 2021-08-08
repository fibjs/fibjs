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

        v8::Local<v8::Function> _then_func;
        v8::Local<v8::Function> _catch_func;

        _then_func = isolate->NewFunction("promise_then", promise_then, _data);
        if (_then_func.IsEmpty()) {
            ThrowError("function alloc error.");
            return result;
        }
        _catch_func = isolate->NewFunction("promise_catch", promise_catch, _data);
        if (_catch_func.IsEmpty()) {
            ThrowError("function alloc error.");
            return result;
        }

        if (!_promise.IsEmpty()) {
            _promise->Then(context, _then_func);
            _promise->Catch(context, _catch_func);
        } else {
            _then->Call(_then->CreationContext(), result, 1, (v8::Local<v8::Value>*)&_then_func);
            _catch->Call(_catch->CreationContext(), result, 1, (v8::Local<v8::Value>*)&_catch_func);
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