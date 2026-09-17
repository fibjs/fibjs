/*
 * AsyncResource.cpp
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#include "AsyncResource.h"
#include "async_context.h"

namespace fibjs {

static std::atomic<int64_t> s_nextAsyncId { 1 };

static bool async_resource_should_stop(Isolate* isolate)
{
    return !isolate || isolate->is_terminating() || isolate->m_isolate->IsExecutionTerminating();
}

result_t AsyncResource_base::_new(exlib::string type, v8::Local<v8::Value> triggerAsyncId,
    obj_ptr<AsyncResource_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    double asyncId = s_nextAsyncId.fetch_add(1);
    double triggerAsync = 0;

    if (!triggerAsyncId.IsEmpty() && !triggerAsyncId->IsUndefined() && !triggerAsyncId->IsNull()) {
        if (triggerAsyncId->IsNumber()) {
            triggerAsync = triggerAsyncId.As<v8::Number>()->Value();
        } else if (triggerAsyncId->IsObject()) {
            // Options object: { triggerAsyncId, requireManualDestroy }
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Object> opts = triggerAsyncId.As<v8::Object>();
            v8::MaybeLocal<v8::Value> maybeTrigger = opts->Get(context,
                isolate->NewString("triggerAsyncId"));
            v8::Local<v8::Value> tv;
            if (maybeTrigger.ToLocal(&tv)) {
                if (tv->IsNumber())
                    triggerAsync = tv.As<v8::Number>()->Value();
            }
        }
    }

    obj_ptr<AsyncResource> ar = new AsyncResource(asyncId, triggerAsync);

    // Capture the current async context
    v8::Local<v8::Map> ctx = cloneAsyncContext(isolate);
    if (!ctx.IsEmpty())
        ar->m_asyncContext.Reset(isolate->m_isolate, ctx);

    retVal = ar;
    return 0;
}

result_t AsyncResource::asyncId(double& retVal)
{
    retVal = m_asyncId;
    return 0;
}

result_t AsyncResource::triggerAsyncId(double& retVal)
{
    retVal = m_triggerAsyncId;
    return 0;
}

result_t AsyncResource::runInAsyncScope(v8::Local<v8::Function> fn, v8::Local<v8::Value> thisArg,
    OptArgs args, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    if (async_resource_should_stop(isolate))
        return 0;

    v8::Local<v8::Context> context = isolate->context();

    // Save the current context
    v8::Global<v8::Value> priorCtxGlobal;
    v8::Local<v8::Map> priorCtx = getAsyncContext(isolate);
    if (!priorCtx.IsEmpty())
        priorCtxGlobal.Reset(isolate->m_isolate, priorCtx);

    // Restore the captured context
    if (!m_asyncContext.IsEmpty()) {
        v8::Local<v8::Map> capturedCtx = m_asyncContext.Get(isolate->m_isolate);
        setAsyncContext(isolate, capturedCtx);
    } else {
        setAsyncContext(isolate, v8::Local<v8::Map>());
    }

    // Prepare arguments
    std::vector<v8::Local<v8::Value>> callArgs;
    for (int i = 0; i < args.Length(); i++)
        callArgs.push_back(args[i]);

    // Determine thisArg: use global if undefined
    v8::Local<v8::Value> receiver = thisArg;
    if (receiver.IsEmpty() || receiver->IsUndefined())
        receiver = context->Global();

    // Call the function
    v8::MaybeLocal<v8::Value> result = fn->Call(context, receiver,
        (int)callArgs.size(), callArgs.empty() ? nullptr : callArgs.data());

    // Restore the prior context
    if (!priorCtxGlobal.IsEmpty())
        setAsyncContext(isolate, priorCtxGlobal.Get(isolate->m_isolate).As<v8::Map>());
    else
        setAsyncContext(isolate, v8::Local<v8::Map>());

    if (result.IsEmpty())
        return CALL_E_JAVASCRIPT;

    if (!result.ToLocal(&retVal))
        return CALL_E_JAVASCRIPT;
    return 0;
}

result_t AsyncResource::emitDestroy(obj_ptr<AsyncResource_base>& retVal)
{
    retVal = this;
    return 0;
}

result_t AsyncResource::bind(v8::Local<v8::Function> fn, v8::Local<v8::Value> thisArg,
    v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    // Create data array: [asyncContext (Map or Undefined), fn, thisArg, wrap]
    v8::Local<v8::Array> data = v8::Array::New(isolate->m_isolate, 4);
    if (!m_asyncContext.IsEmpty())
        data->Set(context, 0, m_asyncContext.Get(isolate->m_isolate)).FromMaybe(false);
    else
        data->Set(context, 0, v8::Undefined(isolate->m_isolate)).FromMaybe(false);
    data->Set(context, 1, fn).FromMaybe(false);
    data->Set(context, 2, thisArg).FromMaybe(false);
    data->Set(context, 3, wrap()).FromMaybe(false);

    v8::MaybeLocal<v8::Function> maybeBound = v8::Function::New(context,
        [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = Isolate::current(info);
            if (async_resource_should_stop(isolate))
                return;

            v8::Local<v8::Context> context = isolate->context();

            v8::Local<v8::Value> rawData = info.Data();
            if (rawData.IsEmpty() || !rawData->IsArray())
                return;

            v8::Local<v8::Array> data = rawData.As<v8::Array>();
            v8::Local<v8::Value> asyncCtxVal = data->Get(context, 0).FromMaybe(v8::Local<v8::Value>());
            v8::Local<v8::Value> fnVal = data->Get(context, 1).FromMaybe(v8::Local<v8::Value>());
            v8::Local<v8::Value> thisArg = data->Get(context, 2).FromMaybe(v8::Local<v8::Value>());
            if (fnVal.IsEmpty() || !fnVal->IsFunction())
                return;

            v8::Local<v8::Function> fn = fnVal.As<v8::Function>();
            if (fn.IsEmpty())
                return;

            // Save current context
            v8::Global<v8::Value> priorCtxGlobal;
            v8::Local<v8::Map> priorCtx = getAsyncContext(isolate);
            if (!priorCtx.IsEmpty())
                priorCtxGlobal.Reset(isolate->m_isolate, priorCtx);

            // Restore captured context
            if (asyncCtxVal->IsMap())
                setAsyncContext(isolate, asyncCtxVal.As<v8::Map>());
            else
                setAsyncContext(isolate, v8::Local<v8::Map>());

            // Prepare arguments
            std::vector<v8::Local<v8::Value>> args;
            for (int i = 0; i < info.Length(); i++)
                args.push_back(info[i]);

            // Determine receiver
            v8::Local<v8::Value> receiver = thisArg;
            if (receiver.IsEmpty() || receiver->IsUndefined())
                receiver = info.This();

            v8::MaybeLocal<v8::Value> result = fn->Call(context, receiver,
                (int)args.size(), args.empty() ? nullptr : args.data());

            // Restore context
            if (!priorCtxGlobal.IsEmpty())
                setAsyncContext(isolate, priorCtxGlobal.Get(isolate->m_isolate).As<v8::Map>());
            else
                setAsyncContext(isolate, v8::Local<v8::Map>());

            v8::Local<v8::Value> resolved;
            if (result.ToLocal(&resolved))
                info.GetReturnValue().Set(resolved);
        },
        data);

    if (maybeBound.IsEmpty())
        return CALL_E_JAVASCRIPT;

    v8::Local<v8::Function> boundFn;
    if (!maybeBound.ToLocal(&boundFn))
        return CALL_E_JAVASCRIPT;

    // Set asyncResource property on the bound function
    boundFn->Set(context, isolate->NewString("asyncResource"), wrap()).FromMaybe(false);

    retVal = boundFn;
    return 0;
}

result_t AsyncResource_base::bind(v8::Local<v8::Function> fn, exlib::string type,
    v8::Local<v8::Value> thisArg, v8::Local<v8::Function>& retVal)
{
    // Create an internal AsyncResource
    obj_ptr<AsyncResource_base> ar;
    result_t hr = _new(type, v8::Local<v8::Value>(), ar);
    if (hr < 0)
        return hr;

    // Use the instance bind method
    return static_cast<AsyncResource*>((AsyncResource_base*)ar)->bind(fn, thisArg, retVal);
}

}
