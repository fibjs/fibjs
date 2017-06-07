/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"

#include "ifs/vm.h"
#include "ifs/fs.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "path.h"
#include "loaders/loaders.h"
#include "Buffer.h"

namespace fibjs {

#define FILE_ONLY 1
#define NO_SEARCH 2
#define FULL_SEARCH 3

exlib::string s_root;

void init_sandbox()
{
    if (s_root.empty())
        process_base::cwd(s_root);
}

SandBox::SandBox()
{
    m_cache = new LruCache(0, 3000);

    obj_ptr<ExtLoader> loader;

    loader = new JsLoader();
    m_loaders.push_back(loader);

    loader = new JscLoader();
    m_loaders.push_back(loader);

    loader = new JsonLoader();
    m_loaders.push_back(loader);
}

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

void _require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t argc = args.Length();
    if (argc > 1) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }
    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    V8_SCOPE(isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    v8::Local<v8::Value> v;
    hr = sbox->require(*v8::String::Utf8Value(path), id, v, FULL_SEARCH);
    if (hr < 0) {
        if (hr == CALL_E_JAVASCRIPT) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(v));
}

void _run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    int32_t argc = args.Length();

    if (argc > 2) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    if (argc < 1) {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Array> argv;
    if (argc == 2) {
        result_t hr = GetArgumentValue(args[1], argv);
        if (hr < 0) {
            ThrowResult(hr);
            return;
        }
    } else
        argv = v8::Array::New(isolate);

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    obj_ptr<SandBox> sbox = (SandBox*)SandBox_base::getInstance(
        _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2])))) {
        v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));

        exlib::string strPath;

        path_base::dirname(*v8::String::Utf8Value(path), strPath);
        if (strPath.length())
            strPath += PATH_SLASH;
        id = strPath + id;
    }

    hr = sbox->run(id, argv);
    if (hr < 0) {
        if (hr == CALL_E_JAVASCRIPT) {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }
}

SandBox::Context::Context(SandBox* sb, exlib::string id)
    : m_sb(sb)
{
    Isolate* isolate = m_sb->holder();
    m_id = isolate->NewFromUtf8(id);

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(isolate->NewFromUtf8("_sbox"), m_sb->wrap());
    _mod->Set(isolate->NewFromUtf8("_id"), m_id);

    m_fnRequest = isolate->NewFunction("require", _require, _mod);
    m_fnRun = isolate->NewFunction("run", _run, _mod);
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

result_t SandBox::loadFile(exlib::string fname, obj_ptr<Buffer_base>& data)
{
    result_t hr;
    v8::Local<v8::Value> v;

    m_cache->get(fname, v);
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
        m_cache->set(fname, data->wrap());
    else
        m_cache->set(fname, v8::Null(holder()->m_isolate));

    return hr;
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

result_t SandBox::run_main(exlib::string fname, v8::Local<v8::Array> argv)
{
    result_t hr;

    exlib::string sfname = s_root;

    pathAdd(sfname, fname);
    path_base::normalize(sfname, sfname);

    obj_ptr<ExtLoader> l;
    hr = get_loader(sfname, l);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> bin;

    hr = loadFile(sfname, bin);
    if (hr < 0)
        return hr;

    Context context(this, sfname);
    return l->run_main(&context, bin, sfname, argv);
}

result_t SandBox::run_worker(exlib::string fname, Worker_base* master)
{
    result_t hr;

    exlib::string sfname = s_root;

    pathAdd(sfname, fname);
    path_base::normalize(sfname, sfname);

    obj_ptr<ExtLoader> l;
    hr = get_loader(sfname, l);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> bin;

    hr = loadFile(sfname, bin);
    if (hr < 0)
        return hr;

    Context context(this, sfname);
    return l->run_worker(&context, bin, sfname, master);
}

result_t SandBox::run(exlib::string fname, v8::Local<v8::Array> argv)
{
    result_t hr;

    exlib::string sfname = s_root;

    pathAdd(sfname, fname);
    path_base::normalize(sfname, sfname);

    obj_ptr<ExtLoader> l;
    hr = get_loader(sfname, l);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> bin;

    hr = loadFile(sfname, bin);
    if (hr < 0)
        return hr;

    Context context(this, sfname);
    return l->run_script(&context, bin, sfname, argv);
}
}
