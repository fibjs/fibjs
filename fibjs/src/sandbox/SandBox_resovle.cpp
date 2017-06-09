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

exlib::string s_root;
extern bool g_root;

result_t SandBox::loadFile(exlib::string fname, obj_ptr<Buffer_base>& data)
{
    result_t hr;
    v8::Local<v8::Value> v;
    Isolate* isolate = holder();

    isolate->m_script_cache->get(fname, v);
    if (!v.IsEmpty()) {
        data = Buffer_base::getInstance(v);
        return data ? 0 : CALL_E_FILE_NOT_FOUND;
    }

    hr = fs_base::cc_readFile(fname, data);
    if (hr == CALL_RETURN_NULL) {
        data = new Buffer();
        hr = 0;
    }

    if (data)
        isolate->m_script_cache->set(fname, data->wrap());
    else
        isolate->m_script_cache->set(fname, v8::Null(holder()->m_isolate));

    return hr;
}

result_t SandBox::resovleFile(exlib::string& fname, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>* retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> _mods;
    size_t cnt = m_loaders.size();
    result_t hr;

    if (retVal)
        _mods = mods();

    for (int32_t step = 0; step < 2; step++) {
        exlib::string fname1;
        bool bFound = false;

        for (size_t i = 0; i < cnt; i++) {
            obj_ptr<ExtLoader>& l = m_loaders[i];

            if ((fname.length() > l->m_ext.length())
                && !qstrcmp(fname.c_str() + fname.length() - l->m_ext.length(), l->m_ext.c_str())) {
                bFound = true;
                break;
            }
        }

        if (bFound) {
            if (retVal) {
                *retVal = _mods->Get(isolate->NewFromUtf8(fname));
                if (!IsEmpty(*retVal))
                    return 0;
            }

            hr = loadFile(fname, data);
            if (hr >= 0)
                return 0;
        }

        for (size_t i = 0; i < cnt; i++) {
            obj_ptr<ExtLoader>& l = m_loaders[i];
            fname1 = fname + l->m_ext;

            if (retVal) {
                *retVal = _mods->Get(isolate->NewFromUtf8(fname1));
                if (!IsEmpty(*retVal))
                    return 0;
            }

            hr = loadFile(fname1, data);
            if (hr >= 0) {
                fname = fname1;
                return 0;
            }
        }

        if (step == 0) {
            obj_ptr<Buffer_base> bin;

            fname1 = fname + PATH_SLASH + "package.json";
            hr = loadFile(fname1, bin);
            if (hr < 0) {
                fname1 = fname + ".zip$" + PATH_SLASH + "package.json";
                hr = loadFile(fname1, bin);
                if (hr >= 0)
                    fname = fname + ".zip$";
            }

            if (hr >= 0) {
                v8::Local<v8::Value> v;
                exlib::string buf;

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
                    pathAdd(fname, *v8::String::Utf8Value(main));
                    path_base::normalize(fname, fname);
                } else
                    fname = fname + PATH_SLASH + "index";
            } else
                fname = fname + PATH_SLASH + "index";
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resovleModule(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> _mods = mods();

    retVal = _mods->Get(isolate->NewFromUtf8(id));
    if (!IsEmpty(retVal))
        return 1;

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

    result_t hr;
    exlib::string fname;

    if (g_root) {
        fname = s_root;
        pathAdd(fname, id);

        hr = resovleFile(fname, data, &retVal);
        if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND) {
            id = fname;
            return hr;
        }
    }

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

            hr = resovleFile(fname, data, &retVal);
            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND) {
                id = fname;
                return hr;
            }
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resovle(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
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
            return resovleModule(base, id, data, retVal);
    }

    return resovleFile(id, data, &retVal);
}

result_t SandBox::resovle(exlib::string id, exlib::string base, exlib::string& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> data;
    v8::Local<v8::Value> v;

    hr = resovle(base, id, data, v);
    if (hr < 0)
        return hr;

    retVal = id;
    return 0;
}
}