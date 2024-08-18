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
#include "Event.h"
#include "path.h"
#include "Buffer.h"
#include "options.h"
#include "loaders/loaders.h"

namespace fibjs {

result_t SandBox::wait_module(v8::Local<v8::Object> module, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
    JSValue l = module->GetPrivate(_context, strPendding);
    if (!l->IsUndefined()) {
        if (l->IsPromise()) {
            isolate->await(v8::Local<v8::Promise>::Cast(l));
        } else {
            obj_ptr<Lock_base> lock = Lock_base::getInstance(l);
            if (lock) {
                bool is_lock = false;
                lock->acquire(true, is_lock);
                lock->release();
            }
        }
    }

    if (!module->Has(_context, strExports).FromMaybe(false))
        return Runtime::setError("SandBox: unknown error during module loading.");

    retVal = module->Get(_context, strExports).FromMaybe(v8::Local<v8::Value>());
    return 0;
}

v8::Local<v8::Object> SandBox::get_module(v8::Local<v8::Object> mods, exlib::string id)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> _context = isolate->context();

    JSValue m = mods->Get(_context, isolate->NewString(id));
    if (m->IsObject())
        return v8::Local<v8::Object>::Cast(m);
    return v8::Local<v8::Object>();
}

result_t SandBox::loadFile(exlib::string fname, obj_ptr<Buffer_base>& data)
{
    Isolate* isolate = holder();
    std::pair<int, obj_ptr<Buffer_base>> result;

    isolate->m_file_cache.lookup(fname, result, [isolate](exlib::string& fname, std::pair<int, obj_ptr<Buffer_base>>& result) -> bool {
        result_t hr;

        if (fname.substr(fname.length() - 5) == ".node") {
            obj_ptr<Stat_base> stat;
            hr = fs_base::cc_stat(fname, stat, isolate);
            result.first = hr;
            if (hr >= 0)
                result.second = new Buffer();

            return true;
        }

        Variant var;
        hr = fs_base::cc_readFile(fname, "", var, isolate);
        if (hr == CALL_RETURN_NULL) {
            result.second = new Buffer();
            result.first = 0;
        } else {
            result.second = (Buffer_base*)var.object();
            result.first = hr;
        }

        return true;
    });

    data = result.second;
    return result.first;
}

result_t SandBox::realpath(exlib::string fname, exlib::string& retVal)
{
    Isolate* isolate = holder();
    std::pair<int, exlib::string> result;

    isolate->m_realpath_cache.lookup(fname, result, [isolate](exlib::string& fname, std::pair<int, exlib::string>& result) -> bool {
        exlib::string retVal;

        result.first = fs_base::cc_realpath(fname, retVal, isolate);
        result.second = retVal;

        return true;
    });

    retVal = result.second;
    return result.first;
}

result_t SandBox::resolveFile(v8::Local<v8::Object> mods, exlib::string& fname, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Object>* retVal)
{
    Isolate* isolate = holder();
    size_t cnt = m_loaders.size();
    result_t hr;
    exlib::string fname1;

    hr = realpath(fname, fname1);
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

        hr = realpath(fname2, fname1);
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
    obj_ptr<Buffer_base>& data, v8::Local<v8::Object>* retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
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
    exlib::string config_name;

    v8::Local<v8::String> strExports = isolate->NewString("exports", 7);
    JSValue exports = o->Get(context, strExports);
    if (!IsEmpty(exports)) {
        v8::Local<v8::String> strRoot = isolate->NewString(".", 1);
        v8::Local<v8::String> strRequire = isolate->NewString("require", 7);
        v8::Local<v8::String> strNode = isolate->NewString("node", 4);
        v8::Local<v8::String> strDefault = isolate->NewString("default", 7);

        std::function<bool(JSValue)> resolve_export;
        resolve_export = [&](JSValue exports) -> bool {
            JSValue def_value;

            if (exports->IsArray()) {
                v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(exports);
                int32_t len = arr->Length();

                for (int32_t i = 0; i < len; i++) {
                    def_value = arr->Get(context, i);
                    if (resolve_export(def_value))
                        return true;
                }
            } else if (exports->IsObject()) {
                o = v8::Local<v8::Object>::Cast(exports);

                def_value = o->Get(context, strRoot);
                if (IsEmpty(def_value))
                    def_value = o->Get(context, strNode);
                if (IsEmpty(def_value))
                    def_value = o->Get(context, strRequire);
                if (IsEmpty(def_value))
                    def_value = o->Get(context, strDefault);
                if (IsEmpty(def_value))
                    return false;

                return resolve_export(def_value);
            } else if (exports->IsString()) {
                config_name = isolate->toString(exports);
                return true;
            }

            return false;
        };

        if (!resolve_export(exports))
            return CHECK_ERROR(Runtime::setError("SandBox: \"exports\" in package.json must be a string or object"));
    } else {
        JSValue main = o->Get(context, isolate->NewString("main", 4));
        if (IsEmpty(main))
            return CALL_E_FILE_NOT_FOUND;

        if (!main->IsString() && !main->IsStringObject())
            return CHECK_ERROR(Runtime::setError("SandBox: \"main\" in package.json must be a string"));

        config_name = isolate->toString(main);
    }

    resolvePath(fname, config_name);
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
    v8::Local<v8::Object>* retVal)
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
    ".node",
    ".wasm"
};

result_t SandBox::setModuleCompiler(exlib::string extname, v8::Local<v8::Function> compiler)
{
    if (extname.empty())
        return CALL_E_INVALIDARG;

    if (extname.c_str()[0] != '.')
        return CALL_E_INVALIDARG;

    for (int32_t i = 0; i < (int32_t)ARRAYSIZE(predefine_exts); i++)
        if (extname == predefine_exts[i])
            return CHECK_ERROR(Runtime::setError("SandBox: '" + extname + "' is reserved extension name!"));

    obj_ptr<ExtLoader> loader;

    find_loader("test" + extname, loader);

    if (!loader) {
        loader = new custom_Loader(extname);
        m_loaders.push_back(loader);
    }
    SetPrivate(SandBox::_get_extloader_pname(extname), compiler);

    return 0;
}

result_t SandBox::custom_resolveId(exlib::string& id, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Value> _require = GetPrivate("require");

    if (_require->IsFunction()) {
        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(_require);

        v8::Local<v8::Value> arg = isolate->NewString(id);
        v8::Local<v8::Value> result = func->Call(func->GetCreationContextChecked(), wrap(), 1, &arg).FromMaybe(v8::Local<v8::Value>());
        if (result.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(result)) {
            retVal = InstallModule(id, result);
            return 0;
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolveId(exlib::string& id, v8::Local<v8::Object>& retVal)
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
    v8::Local<v8::Object>& retVal)
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
            char* _fname = NULL;
            for (int32_t i = 0; i < sz; i++)
                if (buf[i] == PATH_SLASH) {
                    if (!_fname)
                        _fname = fname.data();
                    _fname[i] = '/';
                }
        }
#endif

        for (i = 0; opt_tools[i].name && qstrcmp(opt_tools[i].name, fname.c_str()); i++)
            ;

        if (opt_tools[i].name) {
            opt_tools[i].getDate(data);
            return 0;
        }

        if (isPathSlash(base.c_str()[base.length() - 1]))
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
    v8::Local<v8::Object>& retVal)
{
    if (is_relative(id)) {
        resolvePath(base, id);
        path_base::normalize(base, id);
    } else
        path_base::normalize(id, id);

    bool isAbs;
    path_base::isAbsolute(id, isAbs);
    if (!isAbs) {
        result_t hr;

        hr = resolveId(id, retVal);
        if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
            return hr;
        return resolveModule(base, id, data, retVal);
    }

    return resolveFile(id, data, &retVal);
}

result_t SandBox::resolve(exlib::string id, exlib::string base, exlib::string& retVal)
{
    result_t hr;
    obj_ptr<Buffer_base> data;
    v8::Local<v8::Object> v;

    hr = resolve(base, id, data, v);
    if (hr < 0)
        return hr;

    retVal = id;
    return 0;
}
}
