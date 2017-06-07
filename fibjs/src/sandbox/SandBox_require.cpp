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

inline exlib::string resolvePath(exlib::string base, exlib::string id)
{
    exlib::string fname;

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2])))) {
        if (!base.empty()) {
            exlib::string strPath;

            path_base::dirname(base, strPath);
            if (strPath.length())
                strPath += PATH_SLASH;
            strPath += id;
            path_base::normalize(strPath, fname);

            return fname;
        }
    }

    path_base::normalize(id, fname);
    return fname;
}

result_t SandBox::addScript(exlib::string srcname, Buffer_base* script,
    v8::Local<v8::Value>& retVal)
{
    result_t hr;

    // add to modules
    exlib::string id(srcname);

    obj_ptr<ExtLoader> l;
    hr = get_loader(id, l);
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

    hr = l->run_module(&context, script, srcname, mod, exports);
    if (hr < 0) {
        // delete from modules
        remove(id);
        return hr;
    }

    // use module.exports as result value
    v8::Local<v8::Value> v = mod->Get(strExports);
    if (!exports->Equals(v))
        InstallModule(id, v);

    retVal = v;
    return 0;
}

result_t SandBox::require(exlib::string base, exlib::string id,
    v8::Local<v8::Value>& retVal, int32_t mode)
{
    exlib::string strId = resolvePath(base, id);
    exlib::string fname;
    Isolate* isolate = holder();
    bool is_jsc = false;

    if (strId.length() > 5 && !qstrcmp(strId.c_str() + strId.length() - 5, ".json")) {
        fname = strId;
        strId.resize(strId.length() - 5);
    } else if (strId.length() > 3 && !qstrcmp(strId.c_str() + strId.length() - 3, ".js")) {
        fname = strId;
        strId.resize(strId.length() - 3);
    } else if (strId.length() > 4 && !qstrcmp(strId.c_str() + strId.length() - 4, ".jsc")) {
        is_jsc = true;
        fname = strId;
        strId.resize(strId.length() - 4);
    }

    v8::Local<v8::Object> _mods = mods();

    retVal = _mods->Get(isolate->NewFromUtf8(strId));
    if (!IsEmpty(retVal))
        return 1;

    exlib::string fullname;

    fullname = s_root;
    pathAdd(fullname, strId);

    if (fullname != strId) {
        retVal = _mods->Get(isolate->NewFromUtf8(fullname));
        if (!IsEmpty(retVal))
            return 1;
    }

    v8::Local<v8::Value> func = GetPrivate("require");
    if (!func->IsUndefined()) {
        v8::Local<v8::Value> arg = isolate->NewFromUtf8(strId);
        retVal = v8::Local<v8::Function>::Cast(func)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal)) {
            if (!retVal->IsProxy() && retVal->IsObject() && !object_base::getInstance(retVal))
                util_base::clone(retVal, retVal);
            InstallModule(strId, retVal);

            return 0;
        }
    }

    result_t hr;
    obj_ptr<Buffer_base> bin;
    exlib::string buf;
    exlib::string fname1;

    if (!fname.empty()) {
        fname1 = s_root;
        pathAdd(fname1, fname);
        fullname = fname1;

        hr = loadFile(fname1, bin);
        if (hr >= 0)
            return addScript(fname1, bin, retVal);
    } else {
        fname = fullname + ".js";
        hr = loadFile(fname, bin);
        if (hr >= 0)
            return addScript(fname, bin, retVal);

        fname = fullname + ".jsc";
        hr = loadFile(fname, bin);
        if (hr >= 0)
            return addScript(fname, bin, retVal);

        fname = fullname + ".json";
        hr = loadFile(fname, bin);
        if (hr >= 0)
            return addScript(fname, bin, retVal);

        fname = strId;
    }

    if (mode <= FILE_ONLY)
        return hr;

    fname1 = fullname + PATH_SLASH + "package.json";
    hr = loadFile(fname1, bin);
    if (hr < 0) {
        fname1 = fullname + ".zip$" + PATH_SLASH + "package.json";
        hr = loadFile(fname1, bin);
        if (hr >= 0)
            fullname = fullname + ".zip$";
    }

    if (hr >= 0) {
        v8::Local<v8::Value> v;

        bin->toString(buf);
        hr = json_base::decode(buf, v);
        if (hr < 0)
            return hr;

        if (v.IsEmpty() || !v->IsObject())
            return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
        v8::Local<v8::Value> main = o->Get(isolate->NewFromUtf8("main", 4));
        if (!IsEmpty(main)) {
            if (!main->IsString() && !main->IsStringObject())
                return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));
            fname1 = fullname + PATH_SLASH;
            fname1 += *v8::String::Utf8Value(main);

            if (fname1.length() > 3 && !qstrcmp(fname1.c_str() + fname1.length() - 3, ".js"))
                fname1.resize(fname1.length() - 3);
        } else
            fname1 = fullname + PATH_SLASH + "index";

        hr = require(base, fname1, retVal, NO_SEARCH);
        if (hr < 0)
            return hr;

        InstallModule(strId, retVal);
        return 0;
    }

    fname1 = fullname + PATH_SLASH + "index";
    hr = require(base, fname1, retVal, FILE_ONLY);
    if (hr >= 0) {
        InstallModule(strId, retVal);
        return 0;
    }

    if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
        return hr;

    fname1 = fullname + ".zip$" + PATH_SLASH + "index";
    hr = require(base, fname1, retVal, FILE_ONLY);
    if (hr >= 0) {
        InstallModule(strId, retVal);
        return 0;
    }

    if (mode <= NO_SEARCH)
        return hr;

    if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
        return hr;

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
        return hr;

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
            str1 += fname;
            path_base::normalize(str1, fname1);

            hr = require(base, fname1, retVal, NO_SEARCH);
            if (hr >= 0) {
                InstallModule(strId, retVal);
                return 0;
            }

            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
                return hr;
        }
    }

    return hr;
}

result_t SandBox::require(exlib::string id, exlib::string base, v8::Local<v8::Value>& retVal)
{
    exlib::string sid;
    path_base::normalize(id, sid);
    if (base.empty())
        base = s_root + '/';
    return require(base, sid, retVal, FULL_SEARCH);
}
}