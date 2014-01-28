/*
 * SandBox.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"
#include "ifs/vm.h"

namespace fibjs
{

result_t vm_base::create(v8::Local<v8::Object> mods,
                         obj_ptr<SandBox_base> &retVal)
{
    obj_ptr<SandBox_base> sbox = new SandBox();
    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;

    return 0;
}

result_t vm_base::create(v8::Local<v8::Object> mods,
                         v8::Local<v8::Function> require, obj_ptr<SandBox_base> &retVal)
{
    obj_ptr<SandBox> sbox = new SandBox();
    sbox->initRequire(require);
    result_t hr = sbox->add(mods);
    if (hr < 0)
        return hr;

    retVal = sbox;

    return 0;
}

result_t vm_base::current(obj_ptr<SandBox_base> &retVal)
{
    v8::Local<v8::Context> ctx = isolate->GetCallingContext();

    if (ctx.IsEmpty())
        return CALL_E_INVALID_CALL;

    v8::Local<v8::Value> sbox = ctx->Global()->GetHiddenValue(
                                        v8::String::NewFromUtf8(isolate, "SandBox"));

    if (sbox.IsEmpty())
        return CALL_E_INTERNAL;

    retVal = SandBox_base::getInstance(sbox);
    return retVal ? 0 : CALL_E_INTERNAL;
}

void SandBox::InstallModule(std::string fname, v8::Local<v8::Value> o,
                            date_t check, date_t mtime)
{
    std::map<std::string, obj_ptr<mod> >::iterator it = m_mods.find(fname);
    obj_ptr<mod> m;

    if (it == m_mods.end())
    {
        m = new mod();
        m_mods[fname] = m;
    }
    else
    {
        m = it->second;
        m->m_mod.Reset();
    }

    m->m_mod.Reset(isolate, o);
    m->m_mtime = mtime;
    m->m_check = check;
}

result_t SandBox::add(const char *id, v8::Local<v8::Value> mod)
{
    if (mod->IsObject())
        mod = mod->ToObject()->Clone();
    InstallModule(id, mod);

    return 0;
}

result_t SandBox::add(v8::Local<v8::Object> mods)
{
    v8::Local<v8::Array> ks = mods->GetPropertyNames();
    int len = ks->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);

        if (!k->IsNumber())
            add(*v8::String::Utf8Value(k), mods->Get(k));
    }

    return 0;
}

result_t SandBox::remove(const char *id)
{
    m_mods.erase(id);
    return 0;
}

} /* namespace fibjs */
