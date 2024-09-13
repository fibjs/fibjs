/*
 * function.cpp
 *
 *  Created on: Oct 13, 2023
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

static v8::Intercepted PropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> sandbox = context->GetEmbedderData(kSandboxObject).As<v8::Object>();

    v8::MaybeLocal<v8::Value> maybe_rv = sandbox->GetRealNamedProperty(context, property);
    if (maybe_rv.IsEmpty())
        return v8::Intercepted::kNo;

    v8::Local<v8::Value> rv;
    if (maybe_rv.ToLocal(&rv)) {
        if (rv == sandbox)
            rv = context->Global();

        args.GetReturnValue().Set(rv);

        return v8::Intercepted::kYes;
    }

    return v8::Intercepted::kNo;
}

static v8::Intercepted PropertySetterCallback(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> sandbox = context->GetEmbedderData(kSandboxObject).As<v8::Object>();

    sandbox->Set(context, property, value).IsJust();
    args.GetReturnValue().Set(value);

    return v8::Intercepted::kYes;
}

static v8::Intercepted PropertyDescriptorCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> sandbox = context->GetEmbedderData(kSandboxObject).As<v8::Object>();

    if (sandbox->HasOwnProperty(context, property).FromMaybe(false)) {
        v8::Local<v8::Value> desc;
        sandbox->GetOwnPropertyDescriptor(context, property).ToLocal(&desc);
        args.GetReturnValue().Set(desc);

        return v8::Intercepted::kYes;
    }

    return v8::Intercepted::kNo;
}

static v8::Intercepted PropertyDeleterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> sandbox = context->GetEmbedderData(kSandboxObject).As<v8::Object>();

    if (sandbox->HasRealNamedProperty(context, property).FromMaybe(false)) {
        sandbox->Delete(context, property).IsJust();
        args.GetReturnValue().Set(v8::True(args.GetIsolate()));
        return v8::Intercepted::kYes;
    }

    return v8::Intercepted::kNo;
}

static void PropertyEnumeratorCallback(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> sandbox = context->GetEmbedderData(kSandboxObject).As<v8::Object>();

    v8::Local<v8::Array> properties;

    sandbox->GetOwnPropertyNames(context).ToLocal(&properties);
    args.GetReturnValue().Set(properties);
}

static v8::Intercepted PropertyDefinerCallback(v8::Local<v8::Name> property, const v8::PropertyDescriptor& desc,
    const v8::PropertyCallbackInfo<void>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> sandbox = context->GetEmbedderData(kSandboxObject).As<v8::Object>();

    auto define_prop_on_sandbox =
        [&](v8::PropertyDescriptor* desc_for_sandbox) {
            if (desc.has_enumerable())
                desc_for_sandbox->set_enumerable(desc.enumerable());

            if (desc.has_configurable())
                desc_for_sandbox->set_configurable(desc.configurable());

            sandbox->DefineProperty(context, property, *desc_for_sandbox);
        };

    if (desc.has_get() || desc.has_set()) {
        v8::PropertyDescriptor desc_for_sandbox(
            desc.has_get() ? desc.get() : v8::Undefined(isolate->m_isolate).As<v8::Value>(),
            desc.has_set() ? desc.set() : v8::Undefined(isolate->m_isolate).As<v8::Value>());

        define_prop_on_sandbox(&desc_for_sandbox);
    } else {
        v8::Local<v8::Value> value = desc.has_value() ? desc.value() : v8::Undefined(isolate->m_isolate).As<v8::Value>();

        if (desc.has_writable()) {
            v8::PropertyDescriptor desc_for_sandbox(value, desc.writable());
            define_prop_on_sandbox(&desc_for_sandbox);
        } else {
            v8::PropertyDescriptor desc_for_sandbox(value);
            define_prop_on_sandbox(&desc_for_sandbox);
        }
    }

    return v8::Intercepted::kYes;
}

void Isolate::init_global_template()
{
    v8::Local<v8::Context> _context = v8::Context::New(m_isolate);
    v8::Context::Scope context_scope(_context);

    v8::Local<v8::FunctionTemplate> global_func_template = v8::FunctionTemplate::New(m_isolate);
    v8::Local<v8::ObjectTemplate> global_object_template = global_func_template->InstanceTemplate();

    global_object_template->SetHandler(v8::NamedPropertyHandlerConfiguration(
        PropertyGetterCallback,
        PropertySetterCallback,
        PropertyDescriptorCallback,
        PropertyDeleterCallback,
        PropertyEnumeratorCallback,
        PropertyDefinerCallback,
        {},
        v8::PropertyHandlerFlags::kHasNoSideEffect));

    m_global_template.Reset(m_isolate, global_object_template);
}

}
