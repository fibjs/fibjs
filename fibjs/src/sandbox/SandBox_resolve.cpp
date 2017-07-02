/*
 * SandBox_file.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/encoding.h"
#include "ifs/fs.h"
#include "ifs/util.h"
#include "path.h"
#include "LruCache.h"
#include "Buffer.h"

namespace fibjs {

result_t SandBox::loadFile(exlib::string fname, obj_ptr<Buffer_base>& data)
{
    result_t hr;
    v8::Local<v8::Value> v;
    Isolate* isolate = holder();

    isolate->m_script_cache->get(fname, v);
    if (!v.IsEmpty()) {
        data = Buffer_base::getInstance(v);
        return data ? 0 : CHECK_ERROR(CALL_E_FILE_NOT_FOUND);
    }

    Variant var;
    hr = fs_base::cc_readFile(fname, "", var);
    if (hr == CALL_RETURN_NULL) {
        data = new Buffer();
        hr = 0;
    } else
        data = Buffer_base::getInstance(var);

    if (data)
        isolate->m_script_cache->set(fname, data->wrap());
    else
        isolate->m_script_cache->set(fname, v8::Null(holder()->m_isolate));

    return hr;
}

result_t SandBox::resolveFile(v8::Local<v8::Object> mods, exlib::string& fname, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>* retVal)
{
    Isolate* isolate = holder();
    size_t cnt = m_loaders.size();
    result_t hr;
    exlib::string fname1;

    if (retVal) {
        *retVal = mods->Get(isolate->NewFromUtf8(fname));
        if (!IsEmpty(*retVal))
            return 0;
    }

    hr = loadFile(fname, data);
    if (hr >= 0)
        return 0;

    for (size_t i = 0; i < cnt; i++) {
        obj_ptr<ExtLoader>& l = m_loaders[i];
        fname1 = fname + l->m_ext;

        if (retVal) {
            *retVal = mods->Get(isolate->NewFromUtf8(fname1));
            if (!IsEmpty(*retVal))
                return 0;
        }

        hr = loadFile(fname1, data);
        if (hr >= 0) {
            fname = fname1;
            return 0;
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolvePackage(v8::Local<v8::Object> mods, exlib::string& fname,
    obj_ptr<Buffer_base>& data, v8::Local<v8::Value>* retVal)
{
    Isolate* isolate = holder();
    exlib::string fname1;
    result_t hr;
    v8::Local<v8::Value> v;
    exlib::string buf;
    obj_ptr<Buffer_base> bin;

    fname1 = fname + PATH_SLASH + "package.json";
    hr = loadFile(fname1, bin);
    if (hr < 0)
        return CALL_E_FILE_NOT_FOUND;

    bin->toString(buf);
    hr = json_base::decode(buf, v);
    if (hr < 0)
        return hr;

    if (v.IsEmpty() || !v->IsObject())
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
    v8::Local<v8::Value> main = o->Get(isolate->NewFromUtf8("main", 4));
    if (IsEmpty(main))
        return CALL_E_FILE_NOT_FOUND;

    if (!main->IsString() && !main->IsStringObject())
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

    resolvePath(fname, *v8::String::Utf8Value(main));
    path_base::normalize(fname, fname);

    hr = resolveFile(mods, fname, data, retVal);
    if (hr >= 0)
        return hr;

    resolvePath(fname, "index");
    hr = resolveFile(mods, fname, data, retVal);
    if (hr >= 0)
        return hr;

    return CHECK_ERROR(Runtime::setError("SandBox: main script in package.json not found"));
}

result_t SandBox::resolveFile(exlib::string& fname, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>* retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> _mods;
    exlib::string fname1;
    result_t hr;

    if (retVal)
        _mods = mods();

    hr = resolveFile(_mods, fname, data, retVal);
    if (hr >= 0)
        return hr;

    hr = resolvePackage(_mods, fname, data, retVal);
    if (hr != CALL_E_FILE_NOT_FOUND)
        return hr;

    fname1 = fname + PATH_SLASH + "index";
    hr = resolveFile(_mods, fname1, data, retVal);
    if (hr >= 0) {
        fname = fname1;
        return hr;
    }

    fname1 = fname + ".zip$";
    hr = resolvePackage(_mods, fname1, data, retVal);
    if (hr != CALL_E_FILE_NOT_FOUND) {
        fname = fname1;
        return hr;
    }

    fname1 = fname1 + PATH_SLASH + "index";
    hr = resolveFile(_mods, fname1, data, retVal);
    if (hr >= 0) {
        fname = fname1;
        return hr;
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolveId(exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> _mods = mods();
    size_t cnt = m_loaders.size();

    retVal = _mods->Get(isolate->NewFromUtf8(id));
    if (!IsEmpty(retVal))
        return 0;

    for (size_t i = 0; i < cnt; i++) {
        obj_ptr<ExtLoader>& l = m_loaders[i];

        retVal = _mods->Get(isolate->NewFromUtf8(id + l->m_ext));
        if (!IsEmpty(retVal))
            return 0;
    }

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

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolveModule(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
{
    result_t hr;
    exlib::string fname;

    if (!base.empty()) {

        while (true) {
            if (isPathSlash(base[base.length() - 1]))
                fname = base.substr(0, base.length() - 1);
            else
                path_base::dirname(base, fname);
            if (base.length() == fname.length())
                break;

            base = fname;

            if (fname.length())
                fname += PATH_SLASH;
            fname += "node_modules";
            fname += PATH_SLASH;
            fname += id;

            hr = resolveFile(fname, data, &retVal);
            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND) {
                id = fname;
                return hr;
            }
        }
    }

    return CHECK_ERROR(CALL_E_FILE_NOT_FOUND);
}

result_t SandBox::resolve(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
{
    const char* c_str = id.c_str();

    if (c_str[0] == '.' && (isPathSlash(c_str[1]) || (c_str[1] == '.' && isPathSlash(c_str[2])))) {
        exlib::string strPath;

        path_base::dirname(base, strPath);
        resolvePath(strPath, id);
        path_base::normalize(strPath, id);
    } else {
        bool isAbs;

        path_base::normalize(id, id);
        path_base::isAbsolute(id, isAbs);
        if (!isAbs) {
            result_t hr;

            hr = resolveId(id, data, retVal);
            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
                return hr;
            return resolveModule(base, id, data, retVal);
        }
    }

    return resolveFile(id, data, &retVal);
}

result_t SandBox::resolve(exlib::string id, exlib::string base, exlib::string& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> data;
    v8::Local<v8::Value> v;

    hr = resolve(base, id, data, v);
    if (hr < 0)
        return hr;

    retVal = id;
    return 0;
}
}