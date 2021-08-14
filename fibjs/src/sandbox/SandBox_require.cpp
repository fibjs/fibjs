/*
 * SandBox_require.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "path.h"

namespace fibjs {

v8::Local<v8::Value> SandBox::get_module(v8::Local<v8::Object> mods, exlib::string id)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::String> strEntry = isolate->NewString("entry");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    JSValue m = mods->Get(_context, isolate->NewString(id));
    if (m->IsUndefined())
        return m;

    v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(m);
    JSValue o = module->Get(_context, strExports);
    JSValue v = module->GetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, strEntry));
    if (!o->IsUndefined() || !v->IsFunction())
        return o;

    JSFunction func = v8::Local<v8::Function>::Cast(v);
    v8::Local<v8::Value> args[6];

    exlib::string pname;
    path_base::dirname(id, pname);

    Context context(this, id);

    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> mod = v8::Object::New(isolate->m_isolate);

    InstallModule(id, exports, mod);

    args[0] = isolate->NewString(id);
    args[1] = isolate->NewString(pname);
    args[2] = context.m_fnRequest;
    args[3] = context.m_fnRun;
    args[4] = exports;
    args[5] = mod;

    mod->Set(_context, strExports, exports);
    mod->Set(_context, isolate->NewString("require"), args[2]);
    mod->Set(_context, isolate->NewString("run"), args[3]);
    mod->SetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, strEntry), func);

    v8::Local<v8::Object> glob = isolate->context()->Global();
    v = func.Call(_context, glob, 6, args);
    if (v.IsEmpty())
        return v;

    return JSValue(mod->Get(_context, strExports));
}

result_t SandBox::refresh()
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::String> strEntry = isolate->NewString("entry");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Object> modules = mods();
    JSArray names = modules->GetPropertyNames(context);

    for (int32_t i = 0; i < (int32_t)names->Length(); i++) {
        v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(JSValue(modules->Get(context, JSValue(names->Get(context, i)))));
        JSValue v = module->GetPrivate(context, v8::Private::ForApi(isolate->m_isolate, strEntry));
        if (v->IsFunction())
            module->Delete(context, strExports);
    }

    return 0;
}

result_t SandBox::installScript(exlib::string srcname, Buffer_base* script,
    v8::Local<v8::Value>& retVal)
{
    result_t hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(srcname, l);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();
    Context context(this, srcname);
    v8::Local<v8::Context> _context = isolate->context();

    v8::Local<v8::Object> mod;
    v8::Local<v8::Object> exports;

    // cache string
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    exports = v8::Object::New(isolate->m_isolate);

    // module and exports object
    mod = v8::Object::New(isolate->m_isolate);

    // init module
    mod->Set(_context, isolate->NewString("id"), isolate->NewString(srcname));
    mod->Set(_context, strExports, exports);
    mod->Set(_context, isolate->NewString("filename"), isolate->NewString(srcname));
    mod->Set(_context, isolate->NewString("require"), context.m_fnRequest);
    mod->Set(_context, isolate->NewString("run"), context.m_fnRun);

    InstallModule(srcname, exports, mod);

    std::vector<ExtLoader::arg> extarg;
    hr = l->run_module(&context, script, srcname, mod, exports, extarg);
    if (hr < 0) {
        // delete from modules
        remove(srcname);
        return hr;
    }

    // use module.exports as result value
    retVal = JSValue(mod->Get(_context, strExports));
    return 0;
}

result_t SandBox::addScript(exlib::string srcname, Buffer_base* script,
    v8::Local<v8::Value>& retVal)
{
    Scope _scope(this);

    if (is_relative(srcname))
        return CHECK_ERROR(Runtime::setError("SandBox: AddScript does not accept relative path."));

    path_base::normalize(srcname, srcname);
    return installScript(srcname, script, retVal);
}

result_t SandBox::run_module(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> data;

    hr = resolve(base, id, data, retVal);
    if (hr < 0 || !IsEmpty(retVal))
        return hr;
    return installScript(id, data, retVal);
}

result_t SandBox::require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    Scope _scope(this);
    return run_module(id, base, retVal);
}
}