/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/vm.h"
#include "ifs/fs.h"
#include "ifs/Stat.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "path.h"

#include <sstream>

namespace fibjs
{

#define FILE_ONLY   1
#define NO_SEARCH   2
#define FULL_SEARCH 3

static std::string s_root;


void init_sandbox()
{
    process_base::cwd(s_root);
}

inline std::string resolvePath(std::string base, std::string id)
{
    std::string fname;

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
    {
        if (!base.empty())
        {
            std::string strPath;

            path_base::dirname(base.c_str(), strPath);
            if (strPath.length())
                strPath += PATH_SLASH;
            strPath += id;
            path_base::normalize(strPath.c_str(), fname);

            return fname;
        }
    }

    path_base::normalize(id.c_str(), fname);
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

    V8_SCOPE();

    std::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0)
    {
        ThrowResult(hr);
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
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

    std::string id;
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

        std::string strPath;

        path_base::dirname(*v8::String::Utf8Value(path), strPath);
        if (strPath.length())
            strPath += PATH_SLASH;
        id = strPath + id;
    }

    hr = sbox->run(id.c_str(), argv);
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

SandBox::Context::Context(SandBox *sb, const char *id) : m_sb(sb)
{
    Isolate* isolate = m_sb->holder();
    m_id = isolate->NewFromUtf8(id);

    v8::Local<v8::Object> _mod = v8::Object::New(isolate->m_isolate);

    _mod->Set(isolate->NewFromUtf8("_sbox"), m_sb->wrap());
    _mod->Set(isolate->NewFromUtf8("_id"), m_id);

    m_fnRequest = createV8Function("require", isolate->m_isolate, _require, _mod);
    m_fnRun = createV8Function("run", isolate->m_isolate, _run, _mod);
}

result_t SandBox::Context::run(std::string src, const char *name, const char **argNames,
                               v8::Local<v8::Value> *args, int32_t argCount)
{
    Isolate* isolate = m_sb->holder();
    const char *oname = name;

    std::string sname = m_sb->name();
    if (!sname.empty())
    {
        sname.append(oname);
        oname = sname.c_str();
    }

    v8::Local<v8::String> soname = isolate->NewFromUtf8(oname);
    std::string pname;
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

        std::string str("(function(");
        int32_t i;

        for (i = 0; i < argCount; i ++)
        {
            str += argNames[i];
            if (i < argCount - 1)
                str += ',';
        }

        src = str + ",__filename,__dirname,__sbname){" + src + "\n});";

        script = v8::Script::Compile(
                     isolate->NewFromUtf8(src), soname);
        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    v8::Local<v8::Value> v = script->Run();
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    args[argCount] = isolate->NewFromUtf8(name);;
    args[argCount + 1] = isolate->NewFromUtf8(pname);
    args[argCount + 2] = isolate->NewFromUtf8(m_sb->name());
    v = v8::Local<v8::Function>::Cast(v)->Call(v8::Object::New(isolate->m_isolate), argCount + 3, args);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}

result_t SandBox::Context::run(std::string src, const char *name, v8::Local<v8::Array> argv,
                               v8::Local<v8::Value> replFunc)
{
    static const char *names[] = {"require", "run", "argv", "repl"};

    if (replFunc.IsEmpty())
    {
        v8::Local<v8::Value> args[10] = {m_fnRequest, m_fnRun, argv};
        return run(src, name, names, args, ARRAYSIZE(names) - 1);
    } else
    {
        v8::Local<v8::Value> args[10] = {m_fnRequest, m_fnRun, argv, replFunc};
        return run(src, name, names, args, ARRAYSIZE(names));
    }
}

result_t SandBox::Context::run(std::string src, const char *name, v8::Local<v8::Object> module,
                               v8::Local<v8::Object> exports)
{
    static const char *names[] = {"module", "exports", "require", "run"};
    v8::Local<v8::Value> args[10] = {module, exports, m_fnRequest, m_fnRun};

    return run(src, name, names, args, ARRAYSIZE(names));
}

result_t SandBox::addScript(const char *srcname, const char *script,
                            v8::Local<v8::Value> &retVal)
{
    std::string fname(srcname);
    result_t hr;

    // add to modules
    std::string id(fname);

    if (id.length() > 5 && !qstrcmp(id.c_str() + id.length() - 5, ".json"))
    {
        id.resize(id.length() - 5);

        v8::Local<v8::Value> v;
        hr = encoding_base::jsonDecode(script, v);
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
            remove(id.c_str());
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

result_t SandBox::require(std::string base, std::string id,
                          v8::Local<v8::Value> &retVal, int32_t mode)
{
    std::string strId = resolvePath(base, id);
    std::string fname;
    std::map<std::string, VariantEx >::iterator it;
    Isolate* isolate = holder();

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

    v8::Local<v8::Object> _mods = mods();

    retVal = _mods->Get(isolate->NewFromUtf8(strId));
    if (!IsEmpty(retVal))
        return 1;

    std::string fullname;

    fullname = s_root;
    pathAdd(fullname, strId.c_str());

    if (qstrcmp(fullname.c_str(), strId.c_str()))
    {
        retVal = _mods->Get(isolate->NewFromUtf8(fullname));
        if (!IsEmpty(retVal))
            return 1;
    }

    v8::Local<v8::Value> func = _mods->GetHiddenValue(isolate->NewFromUtf8("require"));
    if (!IsEmpty(func))
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
    std::string buf;

    if (!fname.empty())
    {
        std::string fname1;

        fname1 = s_root;
        pathAdd(fname1, fname.c_str());

        hr = fs_base::ac_readFile(fname1.c_str(), buf);
        if (hr >= 0)
            return addScript(fname1.c_str(), buf.c_str(), retVal);
    }
    else
    {
        fname = fullname + ".js";
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
            return addScript(fname.c_str(), buf.c_str(), retVal);

        fname = fullname + ".json";
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
            return addScript(fname.c_str(), buf.c_str(), retVal);

        if (mode <= FILE_ONLY)
            return hr;

        fname = fullname + PATH_SLASH + "package.json";
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
        {
            v8::Local<v8::Value> v;
            hr = encoding_base::jsonDecode(buf.c_str(), v);
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
        std::string str, str1;

        str = base;
        while (true)
        {
            path_base::dirname(str.c_str(), str1);
            if (str.length() == str1.length())
                break;

            str = str1;

            if (str1.length())
                str1 += PATH_SLASH;
            str1 += ".modules";
            str1 += PATH_SLASH;
            str1 += strId;
            path_base::normalize(str1.c_str(), fname);

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

result_t SandBox::require(const char *id, v8::Local<v8::Value> &retVal)
{
    std::string sid;
    path_base::normalize(id, sid);
    return require("", sid, retVal, FULL_SEARCH);
}

result_t SandBox::run(const char *fname, v8::Local<v8::Array> argv, v8::Local<v8::Value> replFunc)
{
    result_t hr;

    std::string sfname = s_root;

    pathAdd(sfname, fname);
    path_base::normalize(sfname.c_str(), sfname);

    const char *pname = sfname.c_str();

    std::string buf;
    hr = fs_base::ac_readFile(pname, buf);
    if (hr < 0)
        return hr;

    if (buf[0] == '#' && buf[1] == '!')
    {
        buf[0] = '/';
        buf[1] = '/';
    }

    Context context(this, pname);
    return context.run(buf, pname, argv, replFunc);
}

result_t SandBox::run(const char *fname, v8::Local<v8::Array> argv)
{
    return run(fname, argv, v8::Local<v8::Value>());
}

}

