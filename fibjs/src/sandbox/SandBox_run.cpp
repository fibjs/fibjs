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
#include "ifs/Stat.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "ifs/global.h"
#include "ifs/zlib.h"
#include "path.h"
#include "Buffer.h"

#include <sstream>

namespace fibjs
{

#define FILE_ONLY   1
#define NO_SEARCH   2
#define FULL_SEARCH 3

static exlib::string s_root;

void init_sandbox()
{
    process_base::cwd(s_root);
}

inline exlib::string resolvePath(exlib::string base, exlib::string id)
{
    exlib::string fname;

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
    {
        if (!base.empty())
        {
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

void _require(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    int32_t argc = args.Length();
    if (argc > 1)
    {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }
    if (argc < 1)
    {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    V8_SCOPE(isolate);

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0)
    {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));
    obj_ptr<SandBox> sbox = (SandBox *)SandBox_base::getInstance(
                                _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    v8::Local<v8::Value> v;
    hr = sbox->require(*v8::String::Utf8Value(path), id, v, FULL_SEARCH);
    if (hr < 0)
    {
        if (hr == CALL_E_JAVASCRIPT)
        {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }

    args.GetReturnValue().Set(V8_RETURN(v));
}

void _run(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate* isolate = args.GetIsolate();
    int32_t argc = args.Length();

    if (argc > 2)
    {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    if (argc < 1)
    {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    exlib::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0)
    {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Array> argv;
    if (argc == 2)
    {
        result_t hr = GetArgumentValue(args[1], argv);
        if (hr < 0)
        {
            ThrowResult(hr);
            return;
        }
    } else
        argv = v8::Array::New(isolate);

    v8::Local<v8::Object> _mod = args.Data()->ToObject();
    obj_ptr<SandBox> sbox = (SandBox *)SandBox_base::getInstance(
                                _mod->Get(v8::String::NewFromUtf8(isolate, "_sbox")));

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
    {
        v8::Local<v8::Value> path = _mod->Get(v8::String::NewFromUtf8(isolate, "_id"));

        exlib::string strPath;

        path_base::dirname(*v8::String::Utf8Value(path), strPath);
        if (strPath.length())
            strPath += PATH_SLASH;
        id = strPath + id;
    }

    hr = sbox->run(id, argv);
    if (hr < 0)
    {
        if (hr == CALL_E_JAVASCRIPT)
        {
            args.GetReturnValue().Set(v8::Local<v8::Value>());
            return;
        }
        ThrowResult(hr);
        return;
    }
}

SandBox::Context::Context(SandBox *sb, exlib::string id) : m_sb(sb)
{
    Isolate* isolate = m_sb->holder();
    m_id = isolate->NewFromUtf8(id);

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(isolate->NewFromUtf8("_sbox"), m_sb->wrap());
    _mod->Set(isolate->NewFromUtf8("_id"), m_id);

    m_fnRequest = createV8Function("require", isolate->m_isolate, _require, _mod);
    m_fnRun = createV8Function("run", isolate->m_isolate, _run, _mod);
}

static const char *s_names[] = {"module", "exports", "require", "run", "argv", "global", "repl"};

result_t SandBox::Context::run(exlib::string src, exlib::string name, v8::Local<v8::Value> *args)
{
    Isolate* isolate = m_sb->holder();
    exlib::string oname = name;

    v8::Local<v8::String> soname = isolate->NewFromUtf8(oname);
    exlib::string pname;
    path_base::dirname(name, pname);

    v8::Local<v8::Script> script;
    {
        TryCatch try_catch;

        {
            v8::ScriptCompiler::Source script_source(
                isolate->NewFromUtf8(src),
                v8::ScriptOrigin(soname));

            if (v8::ScriptCompiler::CompileUnbound(
                        isolate->m_isolate, &script_source).IsEmpty())
                return throwSyntaxError(try_catch);
        }

        exlib::string str("(function(");
        int32_t i;

        for (i = 0; i < ARRAYSIZE(s_names); i ++)
        {
            str += s_names[i];
            str += ',';
        }

        src = str + "__filename,__dirname,__sbname){" + src + "\n});";

        script = v8::Script::Compile(
                     isolate->NewFromUtf8(src), soname);
        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    v8::Local<v8::Value> v = script->Run();
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    args[ARRAYSIZE(s_names)] = isolate->NewFromUtf8(name);;
    args[ARRAYSIZE(s_names) + 1] = isolate->NewFromUtf8(pname);
    args[ARRAYSIZE(s_names) + 2] = isolate->NewFromUtf8(m_sb->m_name);
    v = v8::Local<v8::Function>::Cast(v)->Call(v8::Object::New(isolate->m_isolate), ARRAYSIZE(s_names) + 3, args);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}

result_t SandBox::Context::run(Buffer_base* src, exlib::string name, v8::Local<v8::Value> *args)
{
    result_t hr;

    obj_ptr<Buffer_base> unz;
    hr = zlib_base::cc_gunzip(src, unz);
    if (hr < 0)
        return hr;

    Isolate* isolate = m_sb->holder();
    exlib::string oname = name;

    v8::Local<v8::String> soname = isolate->NewFromUtf8(oname);
    exlib::string pname;
    path_base::dirname(name, pname);

    v8::Local<v8::Script> script;
    {
        TryCatch try_catch;

        exlib::string code;
        unz->toString(code);

        v8::ScriptCompiler::CachedData* cache;
        cache = new v8::ScriptCompiler::CachedData((const uint8_t*)code.c_str(),
                (int32_t)(code.length() - sizeof(int32_t)));

        exlib::string s_temp_source;
        int32_t len = *(int32_t*)(code.c_str() + code.length() - sizeof(int32_t));
        int32_t i;

        s_temp_source.resize(len);

        for (i = 0; i < len; i ++)
            s_temp_source[i] = '.';

        v8::ScriptCompiler::Source source(isolate->NewFromUtf8(s_temp_source),
                                          v8::ScriptOrigin(soname), cache);

        script = v8::ScriptCompiler::Compile(isolate->m_isolate, &source,
                                             v8::ScriptCompiler::kConsumeCodeCache);

        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    v8::Local<v8::Value> v = script->Run();
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    args[ARRAYSIZE(s_names)] = isolate->NewFromUtf8(name);;
    args[ARRAYSIZE(s_names) + 1] = isolate->NewFromUtf8(pname);
    args[ARRAYSIZE(s_names) + 2] = isolate->NewFromUtf8(m_sb->m_name);
    v = v8::Local<v8::Function>::Cast(v)->Call(v8::Object::New(isolate->m_isolate), ARRAYSIZE(s_names) + 3, args);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}

template<typename T>
result_t SandBox::Context::run(T src, exlib::string name, v8::Local<v8::Array> argv, bool main)
{
    Isolate *isolate = m_sb->holder();

    if (!main)
    {
        // "module", "exports", "require", "run", "argv", "global", "repl"
        v8::Local<v8::Value> args[10] =
        {
            v8::Undefined(isolate->m_isolate),
            v8::Undefined(isolate->m_isolate),
            m_fnRequest,
            m_fnRun,
            argv,
            v8::Undefined(isolate->m_isolate),
            v8::Undefined(isolate->m_isolate)
        };

        return run(src, name, args);
    } else
    {
        v8::Local<v8::Object> glob = v8::Local<v8::Object>::New(isolate->m_isolate, isolate->m_global);
        v8::Local<v8::Value> replFunc = global_base::class_info().getFunction(isolate)->Get(
                                            isolate->NewFromUtf8("repl"));

        // "module", "exports", "require", "run", "argv", "global", "repl"
        v8::Local<v8::Value> args[10] =
        {
            v8::Undefined(isolate->m_isolate),
            v8::Undefined(isolate->m_isolate),
            m_fnRequest,
            m_fnRun,
            argv,
            glob,
            replFunc
        };
        return run(src, name, args);
    }
}

template<typename T>
result_t SandBox::Context::run(T src, exlib::string name, v8::Local<v8::Object> module,
                               v8::Local<v8::Object> exports)
{
    Isolate *isolate = m_sb->holder();
    // "module", "exports", "require", "run", "argv", "global", "repl"
    v8::Local<v8::Value> args[10] =
    {
        module,
        exports,
        m_fnRequest,
        m_fnRun,
        v8::Undefined(isolate->m_isolate),
        v8::Undefined(isolate->m_isolate),
        v8::Undefined(isolate->m_isolate)
    };

    return run(src, name, args);
}

result_t SandBox::compile(exlib::string srcname, exlib::string script,
                          obj_ptr<Buffer_base>& retVal)
{
    Isolate *isolate = holder();
    exlib::string oname = srcname;

    v8::Local<v8::String> soname = isolate->NewFromUtf8(oname);

    v8::Local<v8::Script> code;
    {
        TryCatch try_catch;

        {
            v8::ScriptCompiler::Source script_source(
                isolate->NewFromUtf8(script));

            if (v8::ScriptCompiler::CompileUnbound(
                        isolate->m_isolate, &script_source).IsEmpty())
                return throwSyntaxError(try_catch);
        }

        exlib::string str("(function(");
        int32_t i;

        for (i = 0; i < ARRAYSIZE(s_names); i ++)
        {
            str += s_names[i];
            str += ',';
        }

        script = str + "__filename,__dirname,__sbname){" + script + "\n});";

        v8::ScriptCompiler::Source script_source(
            isolate->NewFromUtf8(script), v8::ScriptOrigin(soname));

        if (v8::ScriptCompiler::CompileUnbound(
                    isolate->m_isolate, &script_source,
                    v8::ScriptCompiler::kProduceCodeCache).IsEmpty())
            return throwSyntaxError(try_catch);

        const v8::ScriptCompiler::CachedData* cache = script_source.GetCachedData();

        exlib::string buf((const char*)cache->data, cache->length);

        int32_t len = (int32_t)script.length();
        buf.append((const char*)&len, sizeof(len));

        obj_ptr<Buffer_base> unz = new Buffer(buf);
        return zlib_base::cc_gzip(unz, retVal);
    }

    return 0;
}

result_t SandBox::compile(exlib::string script, obj_ptr<Buffer_base>& retVal)
{
    return compile("", script, retVal);
}

result_t SandBox::addScript(exlib::string srcname, exlib::string script,
                            v8::Local<v8::Value> &retVal)
{
    exlib::string fname(srcname);
    result_t hr;

    // add to modules
    exlib::string id(fname);

    if (id.length() > 5 && !qstrcmp(id.c_str() + id.length() - 5, ".json"))
    {
        id.resize(id.length() - 5);

        v8::Local<v8::Value> v;
        hr = json_base::decode(script, v);
        if (hr < 0)
            return hr;

        InstallModule(id, v);

        retVal = v;
        return 0;
    }
    else if (id.length() > 3 && !qstrcmp(id.c_str() + id.length() - 3, ".js"))
    {
        Isolate* isolate = holder();
        Context context(this, srcname);

        id.resize(id.length() - 3);

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

        hr = context.run(script, srcname, mod, exports);
        if (hr < 0)
        {
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
    else
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));

    return 0;
}

result_t SandBox::addScript(exlib::string srcname, Buffer_base* script,
                            v8::Local<v8::Value>& retVal)
{
    exlib::string fname(srcname);
    result_t hr;

    // add to modules
    exlib::string id(fname);

    if ((id.length() > 5 && !qstrcmp(id.c_str() + id.length() - 5, ".json")) ||
            (id.length() > 3 && !qstrcmp(id.c_str() + id.length() - 3, ".js")))
    {
        exlib::string strScript;
        script->toString(strScript);
        return addScript(srcname, strScript, retVal);
    } else if (id.length() > 4 && !qstrcmp(id.c_str() + id.length() - 4, ".jsc"))
    {
        Isolate* isolate = holder();
        Context context(this, srcname);

        id.resize(id.length() - 4);

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

        hr = context.run(script, srcname, mod, exports);
        if (hr < 0)
        {
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
    else
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));

    return 0;
}

result_t SandBox::require(exlib::string base, exlib::string id,
                          v8::Local<v8::Value> &retVal, int32_t mode)
{
    exlib::string strId = resolvePath(base, id);
    exlib::string fname;
    std::map<exlib::string, VariantEx >::iterator it;
    Isolate* isolate = holder();
    bool is_jsc = false;

    if (strId.length() > 5 && !qstrcmp(strId.c_str() + strId.length() - 5, ".json"))
    {
        fname = strId;
        strId.resize(strId.length() - 5);
    }
    else if (strId.length() > 3 && !qstrcmp(strId.c_str() + strId.length() - 3, ".js"))
    {
        fname = strId;
        strId.resize(strId.length() - 3);
    }
    else if (strId.length() > 4 && !qstrcmp(strId.c_str() + strId.length() - 4, ".jsc"))
    {
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

    if (fullname != strId)
    {
        retVal = _mods->Get(isolate->NewFromUtf8(fullname));
        if (!IsEmpty(retVal))
            return 1;
    }

    v8::Local<v8::Value> func = GetPrivate("require");
    if (!func->IsUndefined())
    {
        v8::Local<v8::Value> arg = isolate->NewFromUtf8(strId);
        retVal = v8::Local<v8::Function>::Cast(func)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal))
        {
            if (!retVal->IsProxy() && retVal->IsObject() && !object_base::getInstance(retVal))
                util_base::clone(retVal, retVal);
            InstallModule(strId, retVal);

            return 0;
        }
    }

    result_t hr;
    obj_ptr<Buffer_base> bin;
    exlib::string buf;

    if (!fname.empty())
    {
        exlib::string fname1;

        fname1 = s_root;
        pathAdd(fname1, fname);

        if (is_jsc)
        {
            hr = fs_base::cc_readFile(fname1, bin);
            if (hr >= 0)
                return addScript(fname1, bin, retVal);
        } else
        {
            hr = fs_base::cc_readTextFile(fname1, buf);
            if (hr >= 0)
                return addScript(fname1, buf, retVal);
        }
    }
    else
    {
        fname = fullname + ".js";
        hr = fs_base::cc_readTextFile(fname, buf);
        if (hr >= 0)
            return addScript(fname, buf, retVal);

        fname = fullname + ".jsc";
        hr = fs_base::cc_readFile(fname, bin);
        if (hr >= 0)
            return addScript(fname, bin, retVal);

        fname = fullname + ".json";
        hr = fs_base::cc_readTextFile(fname, buf);
        if (hr >= 0)
            return addScript(fname, buf, retVal);

        if (mode <= FILE_ONLY)
            return hr;

        fname = fullname + PATH_SLASH + "package.json";
        hr = fs_base::cc_readTextFile(fname, buf);
        if (hr >= 0)
        {
            v8::Local<v8::Value> v;
            hr = json_base::decode(buf, v);
            if (hr < 0)
                return hr;

            if (v.IsEmpty() || !v->IsObject())
                return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

            v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
            v8::Local<v8::Value> main = o->Get(isolate->NewFromUtf8("main", 4));
            if (!IsEmpty(main))
            {
                if (!main->IsString() && !main->IsStringObject())
                    return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));
                fname = strId + PATH_SLASH;
                fname += *v8::String::Utf8Value(main);

                if (fname.length() > 3 && !qstrcmp(fname.c_str() + fname.length() - 3, ".js"))
                    fname.resize(fname.length() - 3);
            }
            else
                fname = strId + PATH_SLASH + "index";

            hr = require(base, fname, retVal, FILE_ONLY);
            if (hr < 0)
                return hr;

            InstallModule(strId, retVal);
            return 0;
        }

        fname = strId + PATH_SLASH + "index";
        hr = require(base, fname, retVal, FILE_ONLY);
        if (hr >= 0)
        {
            InstallModule(strId, retVal);
            return 0;
        }
    }

    if (mode <= NO_SEARCH)
        return hr;

    if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
        return hr;

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
        return hr;

    if (!base.empty())
    {
        exlib::string str, str1;

        str = base;
        while (true)
        {
            path_base::dirname(str, str1);
            if (str.length() == str1.length())
                break;

            str = str1;

            if (str1.length())
                str1 += PATH_SLASH;
            str1 += ".modules";
            str1 += PATH_SLASH;
            str1 += strId;
            path_base::normalize(str1, fname);

            hr = require(base, fname, retVal, NO_SEARCH);
            if (hr >= 0)
            {
                InstallModule(strId, retVal);
                return 0;
            }

            if (hr != CALL_E_FILE_NOT_FOUND && hr != CALL_E_PATH_NOT_FOUND)
                return hr;
        }
    }

    return hr;
}

result_t SandBox::require(exlib::string id, v8::Local<v8::Value> &retVal)
{
    exlib::string sid;
    path_base::normalize(id, sid);
    return require("", sid, retVal, FULL_SEARCH);
}

result_t SandBox::run(exlib::string fname, v8::Local<v8::Array> argv, bool main)
{
    result_t hr;

    exlib::string sfname = s_root;

    pathAdd(sfname, fname);
    path_base::normalize(sfname, sfname);

    bool is_jsc = false;

    if (sfname.length() > 4 && !qstrcmp(sfname.c_str() + sfname.length() - 4, ".jsc"))
        is_jsc = true;
    else if (sfname.length() > 3 && !qstrcmp(sfname.c_str() + sfname.length() - 3, ".js"))
        ;
    else
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));

    const char *pname = sfname.c_str();

    obj_ptr<Buffer_base> bin;
    exlib::string buf;

    if (is_jsc)
    {
        hr = fs_base::cc_readFile(pname, bin);
        if (hr < 0)
            return hr;

        Context context(this, pname);
        return context.run(bin, pname, argv, main);
    } else
    {
        hr = fs_base::cc_readTextFile(pname, buf);
        if (hr < 0)
            return hr;

        if (buf[0] == '#' && buf[1] == '!')
        {
            buf[0] = '/';
            buf[1] = '/';
        }

        Context context(this, pname);
        return context.run(buf, pname, argv, main);
    }
}

result_t SandBox::run(exlib::string fname, v8::Local<v8::Array> argv)
{
    return run(fname, argv, false);
}

}

