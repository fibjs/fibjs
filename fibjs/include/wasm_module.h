/*
 * wasm_module.h
 *
 * Helpers shared by the `.wasm` module loader (sandbox/loaders/wasm_loader.cpp)
 * and the WASI host (wasm/WASIHost.cpp).
 */

#pragma once

#include "object.h"

namespace fibjs {

// `require('./app.wasm')` returns the instance exports, while `WASI.start()`
// wants the instance that owns them. The loader links the two with this
// well-known symbol (a symbol key is invisible to Object.keys/JSON, and it also
// tells the host that the value really came from a wasm module).
inline v8::Local<v8::Symbol> wasm_instance_symbol(Isolate* isolate)
{
    return v8::Symbol::For(isolate->m_isolate, isolate->NewString("fibjs.wasm.instance"));
}

// Per-isolate map from a wasm import module name to the namespace object used to
// satisfy it. `WASI.registerAsImport()` stores the host here; the loader falls
// back to resolving the name as a module when a name is not registered.
//
// The registry lives on the isolate global object (keyed by a well-known symbol)
// so the V8 heap owns the lifetime: workers get their own registry and nothing
// has to be reset when the isolate goes away.
inline v8::Local<v8::Object> wasm_import_registry(Isolate* isolate, bool create)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Symbol> key = v8::Symbol::For(isolate->m_isolate,
        isolate->NewString("fibjs.wasm.imports"));
    v8::Local<v8::Value> value;

    if (!context->Global()->Get(context, key).ToLocal(&value) || !value->IsObject()) {
        if (!create)
            return v8::Local<v8::Object>();

        value = v8::Object::New(isolate->m_isolate);
        if (!context->Global()->Set(context, key, value).FromMaybe(false))
            return v8::Local<v8::Object>();
    }

    return value.As<v8::Object>();
}
}
