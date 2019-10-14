/*
 * SandBox.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "path.h"
#include "ifs/vm.h"
#include "ifs/util.h"
#include "ifs/test.h"
#include "ifs/Buffer.h"
#include "ifs/EventEmitter.h"
#include "loaders/loaders.h"
#include "options.h"

namespace fibjs {

DECLARE_MODULE(vm);

result_t SandBox_base::_new(v8::Local<v8::Object> mods, obj_ptr<SandBox_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(mods, v8::Local<v8::Function>(), v8::Local<v8::Object>(), retVal, This);
}

result_t SandBox_base::_new(v8::Local<v8::Object> mods, v8::Local<v8::Function> require,
    obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(mods, require, v8::Local<v8::Object>(), retVal, This);
}

result_t SandBox_base::_new(v8::Local<v8::Object> mods, v8::Local<v8::Object> global,
    obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(mods, v8::Local<v8::Function>(), global, retVal, This);
}

result_t SandBox_base::_new(v8::Local<v8::Object> mods, v8::Local<v8::Function> require,
    v8::Local<v8::Object> global, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->wrap(This);

    if (!global.IsEmpty())
        sbox->initGlobal(global);

    if (!require.IsEmpty())
        sbox->initRequire(require);

    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;
    return 0;
}

SandBox::SandBox(bool extLoader)
{
    obj_ptr<ExtLoader> loader;

    loader = new JsLoader();
    m_loaders.push_back(loader);

    if (extLoader) {
        loader = new JscLoader();
        m_loaders.push_back(loader);
    }

    loader = new JsonLoader();
    m_loaders.push_back(loader);

    m_global = false;
}

void SandBox::initGlobal(v8::Local<v8::Object> global)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> _token = isolate->context()->GetSecurityToken();

    v8::Local<v8::Context> _context = v8::Context::New(isolate->m_isolate);
    v8::Context::Scope context_scope(_context);

    _context->SetEmbedderData(1, v8::Object::New(isolate->m_isolate)->GetPrototype());
    _context->SetSecurityToken(_token);

    v8::Local<v8::Object> _global = _context->Global();

    _global->Delete(isolate->NewString("console"));
    _global->Set(isolate->NewString("global"), _global);

    JSArray ks = global->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(i);
        JSValue v = global->Get(k);

        _global->Set(k, v);
    }

    SetPrivate("_global", _global);
    m_global = true;
}

RootModule* RootModule::g_root = NULL;

void SandBox::initRoot()
{
    Isolate* isolate = holder();

    RootModule* pModule = RootModule::g_root;

    while (pModule) {
        InstallModule(pModule->name(), pModule->getModule(isolate));
        pModule = pModule->m_next;
    }

    v8::Local<v8::Object> _emitter = EventEmitter_base::class_info().getModule(isolate);
    _emitter->Set(isolate->NewString("EventEmitter"), _emitter);

    v8::Local<v8::Object> _buffer = Buffer_base::class_info().getModule(isolate);
    _buffer->Set(isolate->NewString("Buffer"), _buffer);
}

result_t SandBox::add(exlib::string id, v8::Local<v8::Value> mod)
{
    if (is_relative(id))
        return CHECK_ERROR(Runtime::setError("SandBox: does not accept relative path."));

    path_base::normalize(id, id);
    util_base::clone(mod, mod);

    InstallModule(id, mod);

    return 0;
}

result_t SandBox::add(v8::Local<v8::Object> mods)
{
    JSArray ks = mods->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(i);
        hr = add(ToCString(v8::String::Utf8Value(k)), mods->Get(k));
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t SandBox::remove(exlib::string id)
{
    path_base::normalize(id, id);
    mods()->Delete(holder()->NewString(id));

    return 0;
}

result_t SandBox::has(exlib::string id, bool& retVal)
{
    path_base::normalize(id, id);
    retVal = mods()->Has(
                       Isolate::current()->context(),
                       holder()->NewString(id))
                 .ToChecked();

    return 0;
}

result_t SandBox::clone(obj_ptr<SandBox_base>& retVal)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->SetPrivate("_mods", mods()->Clone());

    retVal = sbox;

    return 0;
}

result_t deepFreeze(v8::Local<v8::Value> v)
{
    if (v.IsEmpty() || !v->IsObject())
        return 0;

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);

    if (!isFrozen(obj)) {
        obj->SetIntegrityLevel(obj->CreationContext(), v8::IntegrityLevel::kFrozen);
        JSArray names = obj->GetPropertyNames(obj->CreationContext(), v8::KeyCollectionMode::kIncludePrototypes,
            v8::ALL_PROPERTIES, v8::IndexFilter::kIncludeIndices);

        TryCatch try_catch;
        for (int32_t i = 0; i < (int32_t)names->Length(); i++)
            deepFreeze(obj->Get(JSValue(names->Get(i))));
    }

    return 0;
}

result_t SandBox::freeze()
{
    v8::Local<v8::Object> global;
    result_t hr;

    hr = get_global(global);
    if (hr < 0)
        return hr;

    deepFreeze(global);

    return 0;
}

result_t SandBox::get_global(v8::Local<v8::Object>& retVal)
{
    if (!m_global)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = v8::Local<v8::Object>::Cast(GetPrivate("_global"));
    return 0;
}

result_t SandBox::get_modules(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();

    retVal = v8::Object::New(isolate->m_isolate);

    v8::Local<v8::Object> ms = mods();
    JSArray ks = ms->GetPropertyNames();

    v8::Local<v8::String> mgetter = isolate->NewString("exports");
    for (int32_t i = 0, len = ks->Length(); i < len; i++) {
        JSValue k = ks->Get(i);
        retVal->Set(k, JSValue(JSValue(ms->Get(k))->ToObject()->Get(mgetter)));
    }

    return 0;
}

} /* namespace fibjs */
