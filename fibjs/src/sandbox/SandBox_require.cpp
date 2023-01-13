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
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    JSValue m = mods->Get(_context, isolate->NewString(id));
    if (m->IsUndefined())
        return m;

    v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(m);
    JSValue o = module->Get(_context, strExports);
    return o;
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
    v8::Local<v8::String> strModule = isolate->NewString(srcname);

    exports = v8::Object::New(isolate->m_isolate);

    // module and exports object
    mod = v8::Object::New(isolate->m_isolate);

    // init module
    mod->Set(_context, isolate->NewString("id"), strModule).Check();
    mod->Set(_context, strExports, exports).Check();
    mod->Set(_context, isolate->NewString("filename"), strModule).Check();
    mod->Set(_context, isolate->NewString("require"), context.m_fnRequest).Check();
    mod->Set(_context, isolate->NewString("run"), context.m_fnRun).Check();

    v8::Local<v8::Object> _mods = mods();
    _mods->Set(_context, strModule, mod).Check();

    std::vector<ExtLoader::arg> extarg;
    hr = l->run_module(&context, script, srcname, mod, exports, extarg);
    if (hr < 0) {
        // delete from modules
        _mods->Delete(_context, strModule).IsJust();
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