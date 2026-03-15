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
#include "Stat.h"
#include "Buffer.h"
#include "options.h"
#include "loaders/loaders.h"

namespace fibjs {

// Lightweight file type check without creating Stat object
// Returns: 0 = regular file, 1 = directory, -1 = not found/error
static inline int32_t file_type_uncached(const char* path)
{
    // Zip virtual paths (containing '$') are always treated as files
    // since zip doesn't support directory checks
    if (strchr(path, '$') != nullptr)
        return 0;

    struct stat st;
    if (::stat(path, &st) < 0)
        return -1;
    if (S_ISDIR(st.st_mode))
        return 1;
    return 0;
}

// Cached version of file_type for SandBox
int32_t SandBox::file_type(exlib::string fname)
{
    Isolate* isolate = holder();
    int32_t result;

    isolate->m_stat_cache.lookup(fname, result, 
        (LruCache<int32_t>::Resolver)[](exlib::string& fname, int32_t& result) -> bool {
            result = file_type_uncached(fname.c_str());
            return true;
        });

    return result;
}

result_t SandBox::wait_module(v8::Local<v8::Object> module, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
    JSValue l = module->GetPrivate(_context, strPendding);
    if (!l->IsUndefined()) {
        if (l->IsPromise()) {
            isolate->await(l);
        } else {
            obj_ptr<Lock_base> lock = Lock_base::getInstance(l);
            if (lock) {
                bool is_lock = false;
                lock->ac_acquire(true, is_lock);
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
        return m.As<v8::Object>();
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
    size_t cnt = m_loaders.size();
    result_t hr;
    exlib::string fname1;

    // Fast path: check if file exists before expensive realpath call
    int32_t ftype = file_type(fname);
    if (ftype == 0) {
        // File exists, now do realpath
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
    }

    // Try with extensions
    for (size_t i = 0; i < cnt; i++) {
        obj_ptr<ExtLoader>& l = m_loaders[i];
        exlib::string fname2 = fname + l->m_ext;

        // Fast path: check if file with extension exists
        ftype = file_type(fname2);
        if (ftype != 0)
            continue; // Not a file, skip

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

result_t SandBox::resolvePackage(v8::Local<v8::Object> mods, exlib::string module_name, exlib::string script_name,
    obj_ptr<Buffer_base>& data, ModuleType type, exlib::string& out, v8::Local<v8::Object>* retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    exlib::string module_name1;
    result_t hr;
    obj_ptr<Buffer_base> bin;

    module_name1 = module_name;
    resolvePath(module_name1, "package.json");
    hr = loadFile(module_name1, bin);
    if (hr >= 0) {
        v8::Local<v8::Value> v;
        exlib::string buf;

        bin->toString(buf);
        hr = json_base::decode(buf, v);
        if (hr < 0)
            return hr;

        if (v.IsEmpty() || !v->IsObject())
            return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json file '" + module_name1 + "'"));

        v8::Local<v8::Object> o = v.As<v8::Object>();

        v8::Local<v8::String> strExports = isolate->NewString("exports", 7);
        v8::Local<v8::String> strImports = isolate->NewString("imports", 7);
        bool is_internal = script_name[0] == '#';
        JSValue exports = o->Get(context, is_internal ? strImports : strExports);
        if (!IsEmpty(exports)) {
            exlib::string script_part;

            std::function<bool(JSValue)> resolve_export;
            resolve_export = [&](JSValue exports) -> bool {
                JSValue def_value;

                if (exports->IsArray()) {
                    v8::Local<v8::Array> arr = exports.As<v8::Array>();
                    int32_t len = arr->Length();

                    for (int32_t i = 0; i < len; i++) {
                        def_value = arr->Get(context, i);
                        if (resolve_export(def_value))
                            return true;
                    }
                } else if (exports->IsObject()) {
                    o = exports.As<v8::Object>();
                    v8::Local<v8::Array> keys = o->GetPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
                    int32_t len = keys->Length();

                    if (script_name.empty()) {
                        JSValue def_value1;
                        for (int32_t i = 0; i < len; i++) {
                            v8::Local<v8::String> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>())->ToString(context).FromMaybe(v8::Local<v8::String>());
                            exlib::string skey = isolate->toString(key);

                            if (skey == "." || skey == "node" || skey == "default") {
                                def_value = o->Get(context, key);
                                break;
                            } else if (skey == (type == kESModule ? "import" : "require")) {
                                def_value = o->Get(context, key);
                                break;
                            } else if (skey == (type == kESModule ? "require" : "import"))
                                def_value1 = o->Get(context, key);
                        }

                        if (IsEmpty(def_value) && !IsEmpty(def_value1))
                            def_value = def_value1;
                        if (IsEmpty(def_value))
                            return false;
                    } else {
                        for (int32_t i = len - 1; i >= 0; i--) {
                            v8::Local<v8::String> key = keys->Get(context, i).FromMaybe(v8::Local<v8::Value>())->ToString(context).FromMaybe(v8::Local<v8::String>());
                            exlib::string skey = isolate->toString(key);

                            if (skey == "." || skey == "require" || skey == "import" || skey == "node" || skey == "default")
                                continue;

                            path_base::normalize(skey, skey);

                            size_t skey_left_len = skey.find('*');
                            if (skey_left_len != exlib::string::npos) {
                                size_t skey_len = skey.length();
                                size_t skey_right_len = skey_len - skey_left_len - 1;
                                size_t script_name_len = script_name.length();

                                if (!qstrcmp(skey.c_str(), script_name.c_str(), skey_left_len)
                                    && !qstrcmp(skey.c_str() + skey_len - skey_right_len,
                                        script_name.c_str() + script_name_len - skey_right_len, skey_right_len)) {
                                    script_part = script_name.substr(skey_left_len, script_name_len - skey_right_len - skey_left_len);
                                    script_name.clear();
                                    def_value = o->Get(context, key);
                                    break;
                                }
                            } else if (skey == script_name) {
                                script_name.clear();
                                def_value = o->Get(context, key);
                                break;
                            }
                        }

                        if (!script_name.empty())
                            return false;
                    }

                    return resolve_export(def_value);
                } else if (exports->IsString()) {
                    exlib::string match_name = isolate->toString(exports);
                    size_t match_left_len = match_name.find('*');
                    if (match_left_len != exlib::string::npos) {
                        size_t match_len = match_name.length();
                        size_t match_right_len = match_len - match_left_len - 1;

                        script_name = match_name.substr(0, match_left_len) + script_part + match_name.substr(match_len - match_right_len, match_right_len);
                    } else
                        script_name = match_name;

                    return true;
                }

                return false;
            };

            if (!resolve_export(exports)) {
                if (script_name.empty())
                    return CHECK_ERROR(Runtime::setError("SandBox: 'exports' in '" + module_name1 + "' must be a string or object"));
                else
                    return CHECK_ERROR(Runtime::setError("SandBox: Package subpath '" + script_name + "' is not defined by 'exports' in '" + module_name1 + "'"));
            }
        } else if (script_name.empty()) {
            JSValue main = o->Get(context, isolate->NewString("main", 4));
            if (!IsEmpty(main)) {
                if (!main->IsString() && !main->IsStringObject())
                    return CHECK_ERROR(Runtime::setError("SandBox: 'main' in '" + module_name1 + "' must be a string"));

                script_name = isolate->toString(main);
            }
        }
    }

    module_name1 = module_name;
    if (!script_name.empty()) {
        resolvePath(module_name1, script_name);
        path_base::normalize(module_name1, module_name1);

        hr = resolveFile(mods, module_name1, data, retVal);
        if (hr >= 0) {
            out = module_name1;
            return hr;
        }
    }

    exlib::string module_name2 = module_name1;
    resolvePath(module_name2, "index");
    hr = resolveFile(mods, module_name2, data, retVal);
    if (hr >= 0) {
        out = module_name2;
        return hr;
    }

    if (!script_name.empty()) {
        hr = resolvePackage(mods, module_name1, "", data, type, out, retVal);
        if (hr != CALL_E_FILE_NOT_FOUND)
            return hr;

        hr = resolvePackage(mods, module_name1 + ".zip$", "", data, type, out, retVal);
        if (hr != CALL_E_FILE_NOT_FOUND)
            return hr;
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolveModuleType(exlib::string fname, ModuleType& retVal)
{
    if (fname.length() > 4
        && (!qstricmp(fname.c_str() + fname.length() - 4, ".mjs")
            || !qstricmp(fname.c_str() + fname.length() - 4, ".mts"))) {
        retVal = kESModule;
        return 0;
    }

    if (fname.length() < 3
        || (qstricmp(fname.c_str() + fname.length() - 3, ".js")
            && qstricmp(fname.c_str() + fname.length() - 3, ".ts"))) {
        retVal = kCommonJS;
        return 0;
    }

    result_t hr;

    while (true) {
        exlib::string fname1;
        obj_ptr<Buffer_base> bin;

        path_base::dirname(fname, fname1);
        if (fname.length() == fname1.length())
            break;
        fname = fname1;

        resolvePath(fname1, "package.json");
        hr = loadFile(fname1, bin);
        if (hr >= 0) {
            v8::Local<v8::Value> v;
            exlib::string buf;

            bin->toString(buf);
            hr = json_base::decode(buf, v);
            if (hr < 0)
                return hr;

            if (v.IsEmpty() || !v->IsObject())
                return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json file '" + fname1 + "'"));

            exlib::string type;
            v8::Local<v8::Object> o = v.As<v8::Object>();
            GetConfigValue(o, "type", type);

            retVal = type == "module" ? kESModule : kCommonJS;
            return 0;
        }
    }

    retVal = kCommonJS;
    return 0;
}

result_t SandBox::resolveFile(exlib::string module_name, exlib::string script_name, obj_ptr<Buffer_base>& data, ModuleType type,
    exlib::string& out, v8::Local<v8::Object>* retVal)
{
    v8::Local<v8::Object> _mods;
    result_t hr;

    if (retVal)
        _mods = mods();

    if (script_name.empty()) {
        hr = resolveFile(_mods, module_name, data, retVal);
        if (hr >= 0) {
            out = module_name;
            return hr;
        }
    }

    hr = resolvePackage(_mods, module_name, script_name, data, type, out, retVal);
    if (hr != CALL_E_FILE_NOT_FOUND)
        return hr;

    hr = resolvePackage(_mods, module_name + ".zip$", script_name, data, type, out, retVal);
    if (hr != CALL_E_FILE_NOT_FOUND)
        return hr;

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
        return Runtime::setError(CALL_E_INVALIDARG, "SandBox: extension name must not be empty.");

    if (extname[0] != '.')
        return Runtime::setError(CALL_E_INVALIDARG, "SandBox: extension name must start with '.', got '%s'.", extname.c_str());

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
        v8::Local<v8::Function> func = _require.As<v8::Function>();

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

static void split_path(exlib::string path, std::vector<exlib::string>& a)
{
    const char* p = path.c_str();
    int32_t len = (int32_t)path.length();

    int32_t p1, i;

    p1 = 0;
    for (i = 0; i < len; i++) {
        if (isPathSlash(p[i])) {
            a.push_back(exlib::string(p + p1, i - p1));
            p1 = i + 1;
        }
    }
    a.push_back(exlib::string(p + p1, i - p1));
}

result_t SandBox::resolveModule(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data, ModuleType type,
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

        const char* p = fname.c_str();
        if (!qstrcmp(p, "node:", 5)) {
            p += 5;
        } else if (!qstrcmp(p, "fibjs:", 6)) {
            p += 6;
        }
        for (i = 0; opt_tools[i].name && qstrcmp(opt_tools[i].name, p); i++)
            ;

        if (opt_tools[i].name) {
            opt_tools[i].getDate(data);
            if (fname.substr(fname.length() - 5) != ".json")
                id = fname + ".cjs";
            retVal = get_module(mods(), id);
            return 0;
        }

        if (isPathSlash(base[base.length() - 1]))
            base.resize(base.length() - 1);
        fname = base;

        if (id[0] == '#') {
            while (true) {
                base = fname;

                hr = resolveFile(fname, id, data, type, id, &retVal);
                if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
                    return hr;

                path_base::dirname(base, fname);
                if (base.length() == fname.length())
                    break;
            }
        } else {
            std::vector<exlib::string> paths;
            exlib::string module_name;
            exlib::string script_name;

            split_path(id, paths);
            if (paths.size() == 0)
                return CHECK_ERROR(CALL_E_PATH_NOT_FOUND);
            else if (paths.size() == 1) {
                module_name = paths[0];
            } else if (paths[0][0] == '@') {
                module_name = paths[0] + PATH_SLASH + paths[1];

                for (i = 2; i < (int32_t)paths.size(); i++) {
                    if (i > 2)
                        script_name += PATH_SLASH;
                    script_name += paths[i];
                }
            } else {
                module_name = paths[0];

                for (i = 1; i < (int32_t)paths.size(); i++) {
                    if (i > 1)
                        script_name += PATH_SLASH;
                    script_name += paths[i];
                }
            }

            while (true) {
                base = fname;

                exlib::string node_modules_path;
                if (fname.length()) {
                    node_modules_path = fname;
                    resolvePath(node_modules_path, "node_modules");
                } else {
                    node_modules_path = "node_modules";
                }

                // Fast path: skip if node_modules directory doesn't exist
                if (file_type(node_modules_path) != 1) {
                    path_base::dirname(base, fname);
                    if (base.length() == fname.length())
                        break;
                    continue;
                }

                fname = node_modules_path;
                resolvePath(fname, module_name);

                hr = resolveFile(fname, script_name, data, type, id, &retVal);
                if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
                    return hr;

                path_base::dirname(base, fname);
                if (base.length() == fname.length())
                    break;
            }
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t SandBox::resolve(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data, ModuleType type,
    v8::Local<v8::Object>& retVal, v8::Local<v8::Value>* pendding)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> _context = isolate->context();
    exlib::string orig_id = id;
    exlib::string orig_base = base;

    if (is_relative(id)) {
        resolvePath(base, id);
        path_base::normalize(base, id);
    } else
        path_base::normalize(id, id);

    bool isAbs;
    path_base::isAbsolute(id, isAbs);
    if (!isAbs) {
        result_t hr;

        // Optimization: Check resolve cache FIRST before any module lookup
        // Cache key includes module type (CJS/ESM) since the same specifier may resolve to different files
        v8::Local<v8::Object> _resolve_cache = resolve_cache();
        char type_char = (type == kESModule) ? 'E' : 'C';
        exlib::string cache_key = orig_base + '\x00' + type_char + '\x00' + orig_id;
        v8::Local<v8::String> v8_cache_key = isolate->NewString(cache_key);
        v8::Local<v8::Value> cached = _resolve_cache->Get(_context, v8_cache_key).FromMaybe(v8::Local<v8::Value>());
        if (!cached.IsEmpty() && cached->IsString()) {
            // Path cache hit - use cached path directly (like Node.js _pathCache)
            id = isolate->toString(cached);
            
            // Try to get module from mods if already loaded
            v8::Local<v8::Object> _mods = mods();
            retVal = get_module(_mods, id);
            
            // If module is loaded, return immediately
            if (!IsEmpty(retVal)) {
                // Check for pendding promise if module is being loaded
                if (pendding) {
                    v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
                    JSValue p = retVal->GetPrivate(_context, strPendding);
                    if (p->IsPromise())
                        *pendding = p;
                }
                return 0;
            }
            
            // Module not loaded yet, need to load file content for run_module()
            // But path is already resolved, so just load the file
            return loadFile(id, data);
        }

        hr = resolveId(id, retVal);
        if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND) {
            // Check for pendding promise if module is being loaded
            if (pendding && !IsEmpty(retVal)) {
                v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
                JSValue p = retVal->GetPrivate(_context, strPendding);
                if (p->IsPromise())
                    *pendding = p;
            }
            return hr;
        }

        hr = resolveModule(base, id, data, type, retVal);
        if (hr >= 0) {
            // Cache the resolved path
            _resolve_cache->Set(_context, v8_cache_key, isolate->NewString(id)).IsJust();
        }
        return hr;
    }

    result_t hr = resolveFile(id, "", data, type, id, &retVal);
    // Check for pendding promise if module is being loaded
    if (pendding && !IsEmpty(retVal)) {
        v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
        JSValue p = retVal->GetPrivate(_context, strPendding);
        if (p->IsPromise())
            *pendding = p;
    }
    return hr;
}

result_t SandBox::resolve(exlib::string base, exlib::string& id, obj_ptr<Buffer_base>& data,
    v8::Local<v8::Object>& retVal)
{
    return resolve(base, id, data, kCommonJS, retVal);
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
