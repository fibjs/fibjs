/*
 * async_context.h
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 *
 *  Shared async context helpers for AsyncLocalStorage and AsyncResource.
 */

#pragma once

#include "Fiber.h"

namespace fibjs {

// Get the current async context (a JS Map) from fiber-local storage
inline v8::Local<v8::Map> getAsyncContext(Isolate* isolate)
{
    JSFiber* fb = JSFiber::current();
    if (fb && !fb->m_async_ctx.IsEmpty()) {
        v8::Local<v8::Value> data = fb->m_async_ctx.Get(isolate->m_isolate);
        if (data->IsMap())
            return data.As<v8::Map>();
    }
    return v8::Local<v8::Map>();
}

// Set the async context (a JS Map) to fiber-local storage
inline void setAsyncContext(Isolate* isolate, v8::Local<v8::Map> ctx)
{
    JSFiber* fb = JSFiber::current();
    if (fb) {
        if (ctx.IsEmpty())
            fb->m_async_ctx.Reset();
        else
            fb->m_async_ctx.Reset(isolate->m_isolate, ctx);
    }
}

// Clone the current async context (shallow copy of the Map)
inline v8::Local<v8::Map> cloneAsyncContext(Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Map> ctx = getAsyncContext(isolate);
    v8::Local<v8::Map> newCtx = v8::Map::New(isolate->m_isolate);

    if (!ctx.IsEmpty()) {
        v8::Local<v8::Array> entries = ctx->AsArray();
        uint32_t len = entries->Length();
        for (uint32_t i = 0; i < len; i += 2) {
            v8::MaybeLocal<v8::Value> maybeKey = entries->Get(context, i);
            if (maybeKey.IsEmpty())
                return v8::Local<v8::Map>();
            v8::MaybeLocal<v8::Value> maybeValue = entries->Get(context, i + 1);
            if (maybeValue.IsEmpty())
                return v8::Local<v8::Map>();
            v8::Local<v8::Value> key = maybeKey.ToLocalChecked();
            v8::Local<v8::Value> value = maybeValue.ToLocalChecked();
            v8::MaybeLocal<v8::Map> setResult = newCtx->Set(context, key, value);
            if (setResult.IsEmpty())
                return v8::Local<v8::Map>();
        }
    }

    return newCtx;
}

}
