/*
 * AsyncLocalStorage.cpp
 *
 *  Created on: Dec 2, 2025
 *      Author: lion
 */

#include "ifs/async_hooks.h"
#include "AsyncLocalStorage.h"
#include "async_context.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

DECLARE_MODULE(async_hooks);

result_t AsyncLocalStorage_base::_new(v8::Local<v8::Object> options, obj_ptr<AsyncLocalStorage_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);
    obj_ptr<AsyncLocalStorage> als = new AsyncLocalStorage();
    obj_ptr<ALSOptions> opts;
    static std::atomic<int32_t> s_nextId { 0 };

    result_t hr = ALSOptions::load(options, opts);
    if (hr < 0)
        return hr;

    als->m_id = s_nextId.fetch_add(1);
    if (opts->defaultValue.has_value())
        als->m_defaultValue.Reset(isolate->m_isolate, opts->defaultValue.value());
    if (opts->name.has_value()) {
        exlib::string name;
        hr = GetArgumentValue(isolate, opts->name.value(), name);
        if (hr < 0)
            return hr;
        als->m_name = name;
    }

    retVal = als;

    return 0;
}

result_t AsyncLocalStorage_base::snapshot(v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // Capture the current async context (clone the Map)
    v8::Local<v8::Map> capturedCtx = cloneAsyncContext(isolate);

    // Create a bound function that restores the captured context
    v8::MaybeLocal<v8::Function> maybeSnapshotFn = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = Isolate::current(info);
            v8::Local<v8::Context> context = isolate->context();

            if (info.Length() < 1 || !info[0]->IsFunction()) {
                isolate->m_isolate->ThrowException(v8::Exception::TypeError(
                    isolate->NewString("First argument must be a function")));
                return;
            }

            v8::Local<v8::Function> callback = info[0].As<v8::Function>();

            // Get the captured context from the function's data
            v8::Local<v8::Map> capturedCtx = info.Data().As<v8::Map>();

            // Save current context
            v8::Local<v8::Map> priorCtx = getAsyncContext(isolate);

            // Set captured context
            setAsyncContext(isolate, capturedCtx);

            // Prepare arguments (skip the first callback argument)
            std::vector<v8::Local<v8::Value>> args;
            for (int i = 1; i < info.Length(); i++)
                args.push_back(info[i]);

            // Call the callback
            v8::MaybeLocal<v8::Value> result = callback->Call(context, context->Global(),
                (int)args.size(), args.empty() ? nullptr : args.data());

            // Restore prior context
            setAsyncContext(isolate, priorCtx);

            if (!result.IsEmpty())
                info.GetReturnValue().Set(result.ToLocalChecked()); }, capturedCtx);

    if (maybeSnapshotFn.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = maybeSnapshotFn.ToLocalChecked();
    return 0;
}

result_t AsyncLocalStorage_base::bind(v8::Local<v8::Function> fn, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // Capture the current async context (clone the Map)
    v8::Local<v8::Map> capturedCtx = cloneAsyncContext(isolate);

    // Create an array to hold both the captured context and the function
    v8::Local<v8::Array> data = v8::Array::New(isolate->m_isolate, 2);
    data->Set(context, 0, capturedCtx).FromJust();
    data->Set(context, 1, fn).FromJust();

    // Create a bound function that restores the captured context and calls the original function
    v8::MaybeLocal<v8::Function> maybeBoundFn = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value>& info) {
            Isolate* isolate = Isolate::current(info);
            v8::Local<v8::Context> context = isolate->context();

            // Get the data (captured context and function)
            v8::Local<v8::Array> data = info.Data().As<v8::Array>();
            v8::Local<v8::Map> capturedCtx = data->Get(context, 0).ToLocalChecked().As<v8::Map>();
            v8::Local<v8::Function> fn = data->Get(context, 1).ToLocalChecked().As<v8::Function>();

            // Save current context
            v8::Local<v8::Map> priorCtx = getAsyncContext(isolate);

            // Set captured context
            setAsyncContext(isolate, capturedCtx);

            // Prepare arguments
            std::vector<v8::Local<v8::Value>> args;
            for (int i = 0; i < info.Length(); i++)
                args.push_back(info[i]);

            // Call the function
            v8::MaybeLocal<v8::Value> result = fn->Call(context, info.This(),
                (int)args.size(), args.empty() ? nullptr : args.data());

            // Restore prior context
            setAsyncContext(isolate, priorCtx);

            if (!result.IsEmpty())
                info.GetReturnValue().Set(result.ToLocalChecked()); }, data);

    if (maybeBoundFn.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = maybeBoundFn.ToLocalChecked();
    return 0;
}

result_t AsyncLocalStorage::get_name(exlib::string& retVal)
{
    retVal = m_name;
    return 0;
}

result_t AsyncLocalStorage::disable()
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    m_disabled = true;

    // Remove the store from the current context
    v8::Local<v8::Map> ctx = getAsyncContext(isolate);
    if (!ctx.IsEmpty()) {
        v8::Local<v8::Value> key = v8::Integer::New(isolate->m_isolate, m_id);
        ctx->Delete(context, key).FromMaybe(false);
    }

    return 0;
}

result_t AsyncLocalStorage::getStore(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    if (m_disabled) {
        if (!m_defaultValue.IsEmpty())
            retVal = m_defaultValue.Get(isolate->m_isolate);
        return 0;
    }

    v8::Local<v8::Map> ctx = getAsyncContext(isolate);
    if (!ctx.IsEmpty()) {
        v8::Local<v8::Value> key = v8::Integer::New(isolate->m_isolate, m_id);
        // Check if the key exists in the Map
        v8::Maybe<bool> hasKey = ctx->Has(context, key);
        if (hasKey.FromMaybe(false)) {
            // Key exists - return the value (which may be undefined from exit)
            v8::MaybeLocal<v8::Value> maybeValue = ctx->Get(context, key);
            if (!maybeValue.IsEmpty()) {
                v8::Local<v8::Value> value = maybeValue.ToLocalChecked();
                if (!value->IsUndefined()) {
                    retVal = value;
                    return 0;
                }
                // Key exists but value is undefined - this means exit() was called
                // Return undefined, not defaultValue
                return 0;
            }
        }
    }

    // Key doesn't exist in the Map - return defaultValue
    if (!m_defaultValue.IsEmpty())
        retVal = m_defaultValue.Get(isolate->m_isolate);

    return 0;
}

result_t AsyncLocalStorage::enterWith(v8::Local<v8::Value> store)
{
    if (m_disabled)
        m_disabled = false;

    Isolate* isolate = holder();
    isolate->EnsurePromiseHook();
    v8::Local<v8::Context> context = isolate->context();

    // Clone the current context and add/update the store
    v8::Local<v8::Map> newCtx = cloneAsyncContext(isolate);
    if (newCtx.IsEmpty())
        return CALL_E_JAVASCRIPT;
    v8::Local<v8::Value> key = v8::Integer::New(isolate->m_isolate, m_id);
    v8::MaybeLocal<v8::Map> setResult = newCtx->Set(context, key, store);
    if (setResult.IsEmpty())
        return CALL_E_JAVASCRIPT;
    setAsyncContext(isolate, newCtx);

    return 0;
}

result_t AsyncLocalStorage::run(v8::Local<v8::Value> store, v8::Local<v8::Function> callback, OptArgs args, v8::Local<v8::Value>& retVal)
{
    if (m_disabled)
        m_disabled = false;

    Isolate* isolate = holder();
    isolate->EnsurePromiseHook();
    v8::Local<v8::Context> context = isolate->context();

    // Save the current context in a Global to prevent GC issues
    v8::Global<v8::Value> priorCtxGlobal;
    v8::Local<v8::Map> priorCtx = getAsyncContext(isolate);
    if (!priorCtx.IsEmpty())
        priorCtxGlobal.Reset(isolate->m_isolate, priorCtx);

    // Create a new context with the store and store it in a Global
    v8::Global<v8::Map> newCtxGlobal;
    v8::Local<v8::Map> newCtx = cloneAsyncContext(isolate);
    if (newCtx.IsEmpty())
        return CALL_E_JAVASCRIPT;
    v8::Local<v8::Value> key = v8::Integer::New(isolate->m_isolate, m_id);
    v8::MaybeLocal<v8::Map> setResult = newCtx->Set(context, key, store);
    if (setResult.IsEmpty())
        return CALL_E_JAVASCRIPT;
    newCtxGlobal.Reset(isolate->m_isolate, newCtx);
    setAsyncContext(isolate, newCtx);

    // Prepare arguments
    std::vector<v8::Local<v8::Value>> callArgs;
    for (int i = 0; i < args.Length(); i++)
        callArgs.push_back(args[i]);

    // Call the callback
    v8::MaybeLocal<v8::Value> result = callback->Call(context, context->Global(),
        (int)callArgs.size(), callArgs.empty() ? nullptr : callArgs.data());

    // Restore the prior context
    if (!priorCtxGlobal.IsEmpty())
        setAsyncContext(isolate, priorCtxGlobal.Get(isolate->m_isolate).As<v8::Map>());
    else
        setAsyncContext(isolate, v8::Local<v8::Map>());

    if (result.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = result.ToLocalChecked();

    return 0;
}

result_t AsyncLocalStorage::exit(v8::Local<v8::Function> callback, OptArgs args, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    // Save the current context in a Global to prevent GC issues
    v8::Global<v8::Value> priorCtxGlobal;
    v8::Local<v8::Map> priorCtx = getAsyncContext(isolate);
    if (!priorCtx.IsEmpty())
        priorCtxGlobal.Reset(isolate->m_isolate, priorCtx);

    // Create a new context with this ALS's store set to undefined (to mark "exited")
    v8::Global<v8::Map> newCtxGlobal;
    v8::Local<v8::Map> newCtx = cloneAsyncContext(isolate);
    if (newCtx.IsEmpty())
        return CALL_E_JAVASCRIPT;
    v8::Local<v8::Value> key = v8::Integer::New(isolate->m_isolate, m_id);
    // Set to undefined instead of deleting - this signals "exit" state
    newCtx->Set(context, key, v8::Undefined(isolate->m_isolate)).ToLocalChecked();
    newCtxGlobal.Reset(isolate->m_isolate, newCtx);
    setAsyncContext(isolate, newCtx);

    // Prepare arguments
    std::vector<v8::Local<v8::Value>> callArgs;
    for (int i = 0; i < args.Length(); i++)
        callArgs.push_back(args[i]);

    // Call the callback
    v8::MaybeLocal<v8::Value> result = callback->Call(context, context->Global(),
        (int)callArgs.size(), callArgs.empty() ? nullptr : callArgs.data());

    // Restore the prior context
    if (!priorCtxGlobal.IsEmpty())
        setAsyncContext(isolate, priorCtxGlobal.Get(isolate->m_isolate).As<v8::Map>());
    else
        setAsyncContext(isolate, v8::Local<v8::Map>());

    if (result.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = result.ToLocalChecked();

    return 0;
}
}
