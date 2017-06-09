/*
 * SandBox_require.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"

namespace fibjs {

result_t SandBox::addScript(exlib::string srcname, Buffer_base* script,
    v8::Local<v8::Value>& retVal)
{
    result_t hr;

    // add to modules
    exlib::string id(srcname);

    obj_ptr<ExtLoader> l;
    hr = get_loader(srcname, l);
    if (hr < 0)
        return hr;

    id.resize(id.length() - l->m_ext.length());

    Isolate* isolate = holder();
    Context context(this, srcname);

    v8::Local<v8::Object> mod;
    v8::Local<v8::Object> exports;

    // cache string
    v8::Local<v8::String> strRequire = isolate->NewFromUtf8("require");
    v8::Local<v8::String> strExports = isolate->NewFromUtf8("exports");

    exports = v8::Object::New(isolate->m_isolate);

    // module and exports object
    mod = v8::Object::New(isolate->m_isolate);

    // init module
    mod->Set(strExports, exports);
    mod->Set(strRequire, context.m_fnRequest);

    InstallModule(id, exports);
    InstallModule(srcname, exports);

    hr = l->run_module(&context, script, srcname, mod, exports);
    if (hr < 0) {
        // delete from modules
        remove(id);
        remove(srcname);
        return hr;
    }

    // use module.exports as result value
    v8::Local<v8::Value> v = mod->Get(strExports);
    if (!exports->Equals(v)) {
        InstallModule(id, v);
        InstallModule(srcname, v);
    }

    retVal = v;
    return 0;
}

result_t SandBox::require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> data;

    hr = resovle(base, id, data, retVal);
    if (hr < 0 || !IsEmpty(retVal))
        return hr;
    return addScript(id, data, retVal);
}
}