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
#include "options.h"
#include "loaders/loaders.h"

namespace fibjs {

result_t SandBox::loadFile(exlib::string fname, obj_ptr<Buffer_base>& data)
{
    result_t hr;

    Variant var;
    hr = fs_base::cc_readFile(fname, "", var, holder());
    if (hr == CALL_RETURN_NULL) {
        data = new Buffer();
        hr = 0;
    } else
        data = Buffer_base::getInstance(var);

    return hr;
}

result_t SandBox::resolveFile(v8::Local<v8::Object> mods, exlib::string& fname, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>* retVal)
{
    size_t cnt = m_loaders.size();
    result_t hr;
    exlib::string fname1;

    hr = fs_base::cc_realpath(fname, fname1, holder());
    if (hr < 0)
        fname1 = fname;

    if (retVal) {
        *retVal = get_module(mods, fname1);
        if (!IsEmpty(*retVal)) {
            fname = fname1;
            return 0;
        }
    }

    hr = loadFile(fname1, data);
    if (hr >= 0) {
        fname = fname1;
        return 0;
    }

    for (size_t i = 0; i < cnt; i++) {
        obj_ptr<ExtLoader>& l = m_loaders[i];
        exlib::string fname2 = fname + l->m_ext;

        hr = fs_base::cc_realpath(fname2, fname1, holder());
        if (hr < 0)
            fname1 = fname2;

        if (retVal) {
            *retVal = get_module(mods, fname1);
            if (!IsEmpty(*retVal)) {
                fname = fname1;
                return 0;
            }
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

    fname1 = fname;
    resolvePath(fname1, "package.json");
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
    JSValue main = o->Get(isolate->NewString("main", 4));
    if (IsEmpty(main))
        return CALL_E_FILE_NOT_FOUND;

    if (!main->IsString() && !main->IsStringObject())
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

    resolvePath(fname, ToCString(v8::String::Utf8Value(main)));
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

    fname1 = fname;
    resolvePath(fname1, "index");
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

    resolvePath(fname1, "index");
    hr = resolveFile(_mods, fname1, data, retVal);
    if (hr >= 0) {
        fname = fname1;
        return hr;
    }

    return CALL_E_FILE_NOT_FOUND;
}

static const char* predefine_exts[] = {
    ".js",
    ".jsc",
    ".json",
    ".wasm"
};
result_t SandBox::setModuleCompiler(exlib::string extname, v8::Local<v8::Function> compiler)
{
    if (extname.empty())
        return CALL_E_INVALIDARG;

    if (extname[0] != '.')
        return CALL_E_INVALIDARG;

    for (int32_t i = 0; i < (int32_t)ARRAYSIZE(predefine_exts); i++)
        if (extname == predefine_exts[i])
            return CHECK_ERROR(Runtime::setError("SandBox: '" + extname + "' is reserved extension name!"));

    obj_ptr<ExtLoader> loader;

    find_loader("test" + extname, loader);

    if (!loader) {
        loader = new CustomExtLoader(extname);
        m_loaders.push_back(loader);
    }
    SetPrivate(SandBox::_get_extloader_pname(extname), compiler);

    return 0;
}

result_t SandBox::custom_resolveId(exlib::string& id, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Value> func = GetPrivate("require");

    if (!func->IsUndefined()) {
        v8::Local<v8::Value> arg = isolate->NewString(id);
        retVal = v8::Local<v8::Function>::Cast(func)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal)) {
            InstallModule(id, retVal);
            return 0;
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolveId(exlib::string& id, v8::Local<v8::Value>& retVal)
{
    v8::Local<v8::Object> _mods = mods();
    size_t cnt = m_loaders.size();

    retVal = get_module(_mods, id);
    if (!IsEmpty(retVal))
        return 0;

    for (size_t i = 0; i < cnt; i++) {
        obj_ptr<ExtLoader>& l = m_loaders[i];

        retVal = get_module(_mods, id + l->m_ext);
        if (!IsEmpty(retVal))
            return 0;
    }

    return custom_resolveId(id, retVal);
}

result_t SandBox::resolveModule(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
{
    result_t hr;
    exlib::string fname;
    int32_t i;

    if (!base.empty()) {
        fname = id;

        if (fname.substr(fname.length() - 3) == ".js")
            fname.resize(fname.length() - 3);

#ifdef _WIN32
        {
            exlib::string fname1 = fname;
            int32_t sz = (int32_t)fname1.length();
            const char* buf = fname1.c_str();
            for (int32_t i = 0; i < sz; i++)
                if (buf[i] == PATH_SLASH)
                    fname[i] = '/';
        }
#endif

        for (i = 0; opt_tools[i].name && qstrcmp(opt_tools[i].name, fname.c_str()); i++)
            ;

        if (opt_tools[i].name) {
            opt_tools[i].getDate(data);
            return 0;
        }

        if (isPathSlash(base[base.length() - 1]))
            base.resize(base.length() - 1);
        fname = base;

        while (true) {
            base = fname;

            if (fname.length())
                resolvePath(fname, "node_modules");
            else
                fname = "node_modules";
            resolvePath(fname, id);

            hr = resolveFile(fname, data, &retVal);
            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND) {
                id = fname;
                return hr;
            }

            path_base::dirname(base, fname);
            if (base.length() == fname.length())
                break;
        }
    }

    return CHECK_ERROR(CALL_E_FILE_NOT_FOUND);
}

result_t SandBox::resolve(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Value>& retVal)
{
    if (is_relative(id)) {
        resolvePath(base, id);
        path_base::normalize(base, id);
    } else {
        bool isAbs;

        path_base::normalize(id, id);
        path_base::isAbsolute(id, isAbs);
        if (!isAbs) {
            result_t hr;

            hr = resolveId(id, retVal);
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
