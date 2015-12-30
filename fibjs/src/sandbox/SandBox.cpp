/*
 * SandBox.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"
#include "ifs/vm.h"
#include "ifs/util.h"

namespace fibjs
{

result_t SandBox_base::_new(v8::Local<v8::Object> mods, const char *name,
                            obj_ptr<SandBox_base> &retVal, v8::Local<v8::Object> This)
{
    obj_ptr<SandBox> sbox = new SandBox(name);
    sbox->wrap(This);

    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;

    return 0;
}

result_t SandBox_base::_new(v8::Local<v8::Object> mods,
                            v8::Local<v8::Function> require,
                            const char *name,
                            obj_ptr<SandBox_base> &retVal,
                            v8::Local<v8::Object> This)
{
    obj_ptr<SandBox> sbox = new SandBox(name);
    sbox->wrap(This);

    sbox->initRequire(require);
    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;

    return 0;
}

void SandBox::InstallModule(std::string fname, v8::Local<v8::Value> o)
{
    mods()->Set(v8::String::NewFromUtf8(holder()->m_isolate, fname.c_str(), v8::String::kNormalString,
                                        (int32_t)fname.length()), o);
}

result_t SandBox::add(const char *id, v8::Local<v8::Value> mod)
{
    util_base::clone(mod, mod);
    InstallModule(id, mod);

    return 0;
}

result_t SandBox::add(v8::Local<v8::Object> mods)
{
    v8::Local<v8::Array> ks = mods->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);
        add(*v8::String::Utf8Value(k), mods->Get(k));
    }

    return 0;
}

result_t SandBox::remove(const char *id)
{
    mods()->Delete(v8::String::NewFromUtf8(holder()->m_isolate, id));
    return 0;
}

result_t SandBox::clone(obj_ptr<SandBox_base> &retVal)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->m_name = m_name;
    sbox->wrap()->SetHiddenValue(v8::String::NewFromUtf8(holder()->m_isolate, "_mods"),
                                 mods()->Clone());

    retVal = sbox;

    return 0;
}

} /* namespace fibjs */
