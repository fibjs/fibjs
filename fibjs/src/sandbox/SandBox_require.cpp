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

result_t SandBox::installScript(exlib::string srcname, Buffer_base* script,
    v8::Local<v8::Object>& retVal)
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
    mod->Set(_context, isolate->NewString("id"), strModule).IsJust();
    mod->Set(_context, strExports, exports).IsJust();
    mod->Set(_context, isolate->NewString("filename"), strModule).IsJust();
    mod->Set(_context, isolate->NewString("require"), context.m_fnRequest).IsJust();
    mod->Set(_context, isolate->NewString("run"), context.m_fnRun).IsJust();

    v8::Local<v8::Object> _mods = mods();
    _mods->Set(_context, strModule, mod).IsJust();

    obj_ptr<Lock> lock = new Lock();
    v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
    mod->SetPrivate(_context, strPendding, lock->wrap()).IsJust();

    bool is_lock = false;
    lock->acquire(true, is_lock);

    std::vector<ExtLoader::arg> extarg;
    hr = l->run_module(&context, script, srcname, mod, exports, extarg);

    lock->release();
    mod->DeletePrivate(_context, strPendding).IsJust();

    if (hr < 0) {
        // delete from modules
        mod->Delete(_context, strExports).IsJust();
        _mods->Delete(_context, strModule).IsJust();
        return hr;
    }

    // use module.exports as result value
    retVal = mod;
    return 0;
}

result_t SandBox::addScript(exlib::string srcname, Buffer_base* script,
    v8::Local<v8::Value>& retVal)
{
    Scope _scope(this);

    if (is_relative(srcname))
        return CHECK_ERROR(Runtime::setError("SandBox: AddScript does not accept relative path."));

    path_base::normalize(srcname, srcname);

    v8::Local<v8::Object> mod;
    result_t hr = installScript(srcname, script, mod);
    if (hr < 0)
        return hr;

    retVal = wait_module(mod);
    return 0;
}

result_t SandBox::run_module(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> data;
    v8::Local<v8::Object> mod;

    hr = resolve(base, id, data, mod);
    if (hr < 0)
        return hr;
    else if (!IsEmpty(mod)) {
        retVal = wait_module(mod);
        return 0;
    }

    hr = installScript(id, data, mod);
    if (hr < 0)
        return hr;

    retVal = wait_module(mod);
    return 0;
}

result_t SandBox::require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    Scope _scope(this);
    return run_module(id, base, retVal);
}
}