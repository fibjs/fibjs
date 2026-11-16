/*
 * wasm_loader.cpp
 *
 * Loads `.wasm` files as modules. `require('./app.wasm')` compiles the binary,
 * resolves every import namespace, instantiates the module once and exposes the
 * instance exports as the module exports; `import './app.wasm'` reaches the
 * same value through the CommonJS interop (`.wasm` is classified as a data
 * module, so the nearest package.json "type" never turns it into ESM source).
 *
 * V8 seals the `instance.exports` object (it is frozen and non-extensible), so
 * the loader publishes a plain copy instead: same names, same value references
 * (a Memory export stays live), but writable enough to carry the well-known
 * `fibjs.wasm.instance` symbol that lets `WASI.start()` recover the instance.
 *
 * Import namespaces are resolved in this order:
 *   1. the isolate registry filled by `WASI.registerAsImport()` (or any code
 *      that writes the same symbol-keyed object);
 *   2. `require(name)` from the directory of the wasm file, so a namespace can
 *      be a package or a relative path (`import_module("./helper.js")`).
 * A namespace that resolves to neither throws a WebAssembly.LinkError.
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "loaders.h"
#include "wasm_module.h"

namespace fibjs {

// WebAssembly intrinsic lookups: the globals belong to the context the module
// runs in, so nothing here is cached across sandboxes.
static v8::Local<v8::Object> wasm_intrinsic(Isolate* isolate)
{
    v8::Local<v8::Value> value;
    if (!isolate->context()
             ->Global()
             ->Get(isolate->context(), isolate->NewString("WebAssembly"))
             .ToLocal(&value)
        || !value->IsObject())
        return v8::Local<v8::Object>();

    return value.As<v8::Object>();
}

static v8::Local<v8::Value> wasm_member(Isolate* isolate, v8::Local<v8::Object> object,
    const char* name)
{
    v8::Local<v8::Value> value;
    if (object.IsEmpty()
        || !object->Get(isolate->context(), isolate->NewString(name)).ToLocal(&value))
        return v8::Local<v8::Value>();

    return value;
}

// Rethrows a captured V8 exception after prefixing its message with the module
// path, which is what Node does for wasm compile and link failures.
//
// The exception must be captured outside the TryCatch scope: V8 parks a throw
// in the innermost live TryCatch, so rethrowing while one is still alive makes
// the error vanish and the caller sees a silent undefined.
static result_t throw_wasm_error(Isolate* isolate, v8::Local<v8::Value> exception,
    const exlib::string& name)
{
    v8::Local<v8::Context> context = isolate->context();

    if (!exception.IsEmpty() && exception->IsObject()) {
        v8::Local<v8::Object> error = exception.As<v8::Object>();
        v8::Local<v8::Value> message;
        if (error->Get(context, isolate->NewString("message")).ToLocal(&message)
            && message->IsString()) {
            exlib::string text = name;
            text += ": ";
            text += isolate->toString(message);
            error->Set(context, isolate->NewString("message"), isolate->NewString(text)).IsJust();
        }
    }

    v8::Local<v8::Value> to_throw = exception.IsEmpty()
        ? v8::Local<v8::Value>(isolate->NewString(name))
        : exception;
    isolate->m_isolate->ThrowException(to_throw);

    return result_t(CALL_E_JAVASCRIPT);
}

// Swallows the captured exception of a TryCatch and stores it for a throw that
// happens after the handler is gone.
static void take_exception(TryCatch& try_catch, v8::Local<v8::Value>& exception)
{
    exception = try_catch.Exception();
    try_catch.Reset();
}

static result_t throw_link_error(Isolate* isolate, const exlib::string& message)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> ctor = wasm_member(isolate, wasm_intrinsic(isolate), "LinkError");
    v8::Local<v8::Value> error;

    if (ctor->IsFunction()) {
        v8::Local<v8::Value> arg = isolate->NewString(message);
        if (!ctor.As<v8::Function>()->NewInstance(context, 1, &arg).ToLocal(&error))
            error = v8::Local<v8::Value>();
    }

    isolate->m_isolate->ThrowException(error.IsEmpty() ? isolate->NewString(message) : error);
    return result_t(CALL_E_JAVASCRIPT);
}

// A missing module is reported as MODULE_NOT_FOUND by require(); anything else
// (a syntax error inside the namespace module, a throwing getter, ...) is a real
// failure and must not be turned into a LinkError.
static bool is_module_not_found(Isolate* isolate, TryCatch& try_catch)
{
    v8::Local<v8::Value> exception = try_catch.Exception();
    if (exception.IsEmpty() || !exception->IsObject())
        return false;

    v8::Local<v8::Value> code;
    if (!exception.As<v8::Object>()
             ->Get(isolate->context(), isolate->NewString("code"))
             .ToLocal(&code))
        return false;

    return code->IsString() && isolate->toString(code) == "MODULE_NOT_FOUND";
}

result_t wasm_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Isolate* v8_isolate = isolate->m_isolate;

    v8::Local<v8::Object> module = args[2].As<v8::Object>();
    v8::Local<v8::Value> request = args[1];

    v8::Local<v8::Value> module_ctor = wasm_member(isolate, wasm_intrinsic(isolate), "Module");
    v8::Local<v8::Value> instance_ctor = wasm_member(isolate, wasm_intrinsic(isolate), "Instance");
    if (!module_ctor->IsFunction() || !instance_ctor->IsFunction())
        return result_t::make(CALL_E_INVALID_CALL).with_message(
            "WebAssembly is not available in this context.");

    // Compile. The bytes are copied into an ArrayBuffer because the sandbox
    // buffer is released as soon as the loader returns.
    Buffer* buf = Buffer::Cast(src);
    std::shared_ptr<v8::BackingStore> store
        = v8::ArrayBuffer::NewBackingStore(v8_isolate, buf->length());
    if (buf->length() > 0)
        memcpy(store->Data(), buf->data(), buf->length());
    v8::Local<v8::ArrayBuffer> source = v8::ArrayBuffer::New(v8_isolate, store);

    v8::Local<v8::Object> compiled;
    {
        v8::Local<v8::Value> exception;
        {
            TryCatch try_catch;
            v8::Local<v8::Value> arg = source;
            if (!module_ctor.As<v8::Function>()
                     ->NewInstance(context, 1, &arg)
                     .ToLocal(&compiled))
                take_exception(try_catch, exception);
        }
        if (!exception.IsEmpty())
            return throw_wasm_error(isolate, exception, name);
    }

    // Collect the distinct import namespaces of the module.
    std::vector<exlib::string> namespaces;
    {
        v8::Local<v8::Value> list_fn = wasm_member(isolate, module_ctor.As<v8::Object>(), "imports");
        if (!list_fn->IsFunction())
            return result_t::make(CALL_E_INVALID_CALL).with_message(
                "WebAssembly.Module.imports is not available in this context.");

        v8::Local<v8::Value> list;
        v8::Local<v8::Value> exception;
        {
            TryCatch try_catch;
            v8::Local<v8::Value> arg = compiled;
            if (!list_fn.As<v8::Function>()
                     ->Call(context, module_ctor.As<v8::Object>(), 1, &arg)
                     .ToLocal(&list))
                take_exception(try_catch, exception);
        }
        if (!exception.IsEmpty())
            return throw_wasm_error(isolate, exception, name);

        v8::Local<v8::Array> imports = list.As<v8::Array>();
        for (uint32_t i = 0; i < imports->Length(); i++) {
            v8::Local<v8::Value> item;
            if (!imports->Get(context, i).ToLocal(&item) || !item->IsObject())
                continue;

            v8::Local<v8::Value> module_name;
            if (!item.As<v8::Object>()
                     ->Get(context, isolate->NewString("module"))
                     .ToLocal(&module_name)
                || !module_name->IsString())
                continue;

            exlib::string ns = isolate->toString(module_name);
            bool seen = false;
            for (auto& existing : namespaces)
                if (existing == ns) {
                    seen = true;
                    break;
                }
            if (!seen)
                namespaces.push_back(ns);
        }
    }

    // Build the import object, namespace by namespace.
    v8::Local<v8::Object> import_object = v8::Object::New(v8_isolate);
    v8::Local<v8::Object> registry = wasm_import_registry(isolate, false);

    for (auto& ns : namespaces) {
        v8::Local<v8::Value> namespace_value;
        v8::Local<v8::Value> ns_error;
        bool found = false;

        if (!registry.IsEmpty()
            && registry->Get(context, isolate->NewString(ns)).ToLocal(&namespace_value)
            && !namespace_value->IsUndefined() && !namespace_value->IsNull())
            found = true;
        else {
            {
                TryCatch try_catch;
                v8::Local<v8::Value> arg = isolate->NewString(ns);
                if (request->IsFunction()
                    && request.As<v8::Function>()
                           ->Call(context, module, 1, &arg)
                           .ToLocal(&namespace_value))
                    found = true;
                else if (is_module_not_found(isolate, try_catch))
                    try_catch.Reset();
                else
                    take_exception(try_catch, ns_error);
            }

            if (!ns_error.IsEmpty())
                return throw_wasm_error(isolate, ns_error, name);
        }

        if (!found) {
            exlib::string message = "Cannot find wasm import namespace '";
            message += ns;
            message += "' imported from ";
            message += name;
            return throw_link_error(isolate, message);
        }

        import_object->Set(context, isolate->NewString(ns), namespace_value).IsJust();
    }

    // Instantiate and publish the exports.
    v8::Local<v8::Object> instance;
    {
        v8::Local<v8::Value> exception;
        {
            TryCatch try_catch;
            v8::Local<v8::Value> iargs[2] = { compiled, import_object };
            if (!instance_ctor.As<v8::Function>()
                     ->NewInstance(context, 2, iargs)
                     .ToLocal(&instance))
                take_exception(try_catch, exception);
        }
        if (!exception.IsEmpty())
            return throw_wasm_error(isolate, exception, name);
    }

    v8::Local<v8::Value> exports;
    if (!instance->Get(context, isolate->NewString("exports")).ToLocal(&exports)
        || !exports->IsObject())
        return result_t::make(CALL_E_INVALID_CALL).with_message(
            "The wasm module did not produce an exports object.");

    // Copy the sealed exports into a plain object. The values are moved by
    // reference, so a Memory export keeps observing growth, and the copy is the
    // only place the instance link can live.
    v8::Local<v8::Object> export_obj = exports.As<v8::Object>();
    v8::Local<v8::Object> facade = v8::Object::New(v8_isolate);
    {
        v8::Local<v8::Array> keys;
        if (export_obj->GetOwnPropertyNames(context).ToLocal(&keys)) {
            for (uint32_t i = 0; i < keys->Length(); i++) {
                v8::Local<v8::Value> key;
                v8::Local<v8::Value> value;
                if (!keys->Get(context, i).ToLocal(&key) || !key->IsString())
                    continue;
                if (!export_obj->Get(context, key).ToLocal(&value))
                    continue;
                facade->Set(context, key, value).IsJust();
            }
        }
    }

    // Link the instance to its exports so `WASI.start()` can bind an instance
    // when it is handed the value returned by require(). A symbol key does not
    // show up in Object.keys()/JSON.stringify().
    facade->Set(context, wasm_instance_symbol(isolate), instance).Check();

    module->Set(context, isolate->NewString("exports"), facade).IsJust();
    return result_t();
}
}
