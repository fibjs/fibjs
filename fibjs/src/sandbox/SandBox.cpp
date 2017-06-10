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

namespace fibjs {

DECLARE_MODULE(vm);

result_t SandBox_base::_new(v8::Local<v8::Object> mods, obj_ptr<SandBox_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->wrap(This);

    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;

    return 0;
}

result_t SandBox_base::_new(v8::Local<v8::Object> mods,
    v8::Local<v8::Function> require,
    obj_ptr<SandBox_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->wrap(This);

    sbox->initRequire(require);
    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;

    return 0;
}

SandBox::SandBox()
{
    obj_ptr<ExtLoader> loader;

    loader = new JsLoader();
    m_loaders.push_back(loader);

    loader = new JscLoader();
    m_loaders.push_back(loader);

    loader = new JsonLoader();
    m_loaders.push_back(loader);
}
void SandBox::InstallModule(exlib::string fname, v8::Local<v8::Value> o)
{
    mods()->Set(holder()->NewFromUtf8(fname), o);
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

    InstallModule("expect", isolate->NewFunction("expect", test_base::s_expect));

    v8::Local<v8::Object> _emitter = EventEmitter_base::class_info().getModule(isolate);
    _emitter->Set(isolate->NewFromUtf8("EventEmitter"), _emitter);
}

result_t SandBox::add(exlib::string id, v8::Local<v8::Value> mod)
{
    const char* c_str = id.c_str();

    if (c_str[0] == '.' && (isPathSlash(c_str[1]) || (c_str[1] == '.' && isPathSlash(c_str[2]))))
        return CHECK_ERROR(Runtime::setError("SandBox: does not accept relative path."));

    path_base::normalize(id, id);
    util_base::clone(mod, mod);

    InstallModule(id, mod);

    return 0;
}

result_t SandBox::add(v8::Local<v8::Object> mods)
{
    v8::Local<v8::Array> ks = mods->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> k = ks->Get(i);
        hr = add(*v8::String::Utf8Value(k), mods->Get(k));
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t SandBox::remove(exlib::string id)
{
    path_base::normalize(id, id);
    mods()->Delete(holder()->NewFromUtf8(id));

    return 0;
}

result_t SandBox::clone(obj_ptr<SandBox_base>& retVal)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->SetPrivate("_mods", mods()->Clone());

    retVal = sbox;

    return 0;
}

} /* namespace fibjs */
