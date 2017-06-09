/*
 * SandBox_require.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "path.h"

namespace fibjs {

exlib::string s_root;

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

result_t SandBox::requireFile(exlib::string id, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> bin;

    hr = locateFile(id, bin, &retVal);
    if (hr < 0 || !IsEmpty(retVal))
        return hr;

    return addScript(id, bin, retVal);
}

result_t SandBox::requireModule(exlib::string base, exlib::string id, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> _mods = mods();

    retVal = _mods->Get(isolate->NewFromUtf8(id));
    if (!IsEmpty(retVal))
        return 1;

    result_t hr;
    exlib::string fname;

    fname = s_root;
    pathAdd(fname, id);

    hr = requireFile(fname, retVal);
    if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
        return hr;

    v8::Local<v8::Value> func = GetPrivate("require");
    if (!func->IsUndefined()) {
        v8::Local<v8::Value> arg = isolate->NewFromUtf8(id);
        retVal = v8::Local<v8::Function>::Cast(func)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal)) {
            if (!retVal->IsProxy() && retVal->IsObject() && !object_base::getInstance(retVal))
                util_base::clone(retVal, retVal);
            InstallModule(id, retVal);

            return 0;
        }
    }

    if (!base.empty()) {
        exlib::string str, str1;

        str = base;
        while (true) {
            if (isPathSlash(str[str.length() - 1]))
                str1 = str.substr(0, str.length() - 1);
            else
                path_base::dirname(str, str1);
            if (str.length() == str1.length())
                break;

            str = str1;

            if (str1.length())
                str1 += PATH_SLASH;
            str1 += "node_modules";
            str1 += PATH_SLASH;
            str1 += id;

            hr = requireFile(str1, retVal);
            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
                return hr;
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    const char* c_str = id.c_str();

    if (c_str[0] == '.' && (isPathSlash(c_str[1]) || (c_str[1] == '.' && isPathSlash(c_str[2])))) {
        exlib::string strPath;

        path_base::dirname(base, strPath);
        pathAdd(strPath, id);
        path_base::normalize(strPath, id);
    } else {
        bool isAbs;

        path_base::normalize(id, id);
        path_base::isAbsolute(id, isAbs);
        if (!isAbs)
            return requireModule(base, id, retVal);
    }

    return requireFile(id, retVal);
}
}