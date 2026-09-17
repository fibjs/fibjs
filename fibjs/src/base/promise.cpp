/*
 * promise.cpp
 *
 *  Created on: Jun 14, 2023
 *      Author: lion
 */

#include "object.h"
#include "Event.h"

namespace fibjs {

static bool promise_callback_should_stop(v8::Isolate* isolate)
{
    Isolate* fib_isolate = Isolate::current(isolate);
    return !fib_isolate || fib_isolate->is_terminating() || isolate->IsExecutionTerminating();
}

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (promise_callback_should_stop(isolate))
        return;

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Value> data = args.Data();
    if (data.IsEmpty())
        return;

    v8::Local<v8::Object> _data = data.As<v8::Object>();
    if (_data.IsEmpty())
        return;

    _data->Set(context, NewString(isolate, "_result"), args[0]).IsJust();

    obj_ptr<Event_base> ev = Event_base::getInstance(_data);
    ev->set();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (promise_callback_should_stop(isolate))
        return;

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Value> data = args.Data();
    if (data.IsEmpty())
        return;

    v8::Local<v8::Object> _data = data.As<v8::Object>();
    if (_data.IsEmpty())
        return;

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

    METHOD_NAME("promise.await");
    m_allow_module_evaluation_microtasks++;

    // 登记到 isolate：终止时必须被唤醒。等待方 park 在 exlib::Event::wait() 上，
    // 而 promise_then/promise_catch 在终止态下会提前 return、不再 set()，
    // 没有唤醒源就会永久挂住 → worker 退不出去、terminate() 永不收敛。
    // 实测：worker 里 require() 一个稍后才求值完的 ESM 模块（加载器 park 在这里），
    // 此时 terminate() 必挂 —— 中断开/关都会。
    addSyncWaiter(ev);
    ev->ac_wait();
    removeSyncWaiter(ev);

    m_allow_module_evaluation_microtasks--;

    JSValue error = _data->Get(_context, NewString("_error"));
    if (!IsEmpty(error))
        m_isolate->ThrowException(error);
    else
        _data->Get(_context, NewString("_result")).ToLocal(&result);

    return result;
}

}
