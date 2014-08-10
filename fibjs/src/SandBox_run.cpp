/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/vm.h"
#include "ifs/fs.h"
#include "ifs/path.h"
#include "ifs/Stat.h"
#include "ifs/encoding.h"

#include <sstream>

namespace fibjs
{

#define FILE_ONLY   1
#define NO_SEARCH   2
#define FULL_SEARCH 3

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

    std::string id;
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

    args.GetReturnValue().Set(v);
}

void _run(const v8::FunctionCallbackInfo<v8::Value> &args)
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

    std::string id;
    result_t hr = GetArgumentValue(args[0], id);
    if (hr < 0)
    {
        ThrowResult(hr);
        return;
    }

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

    hr = sbox->run(id.c_str());
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

v8::Persistent<v8::Context> s_context_test;

result_t SandBox::Context::run(std::string src, const char *name, const char **argNames,
                               v8::Local<v8::Value> *args, int32_t argCount)
{
    v8::Local<v8::Script> script;
    {
        v8::TryCatch try_catch;

        {
            v8::Local<v8::Context> _context;

            if (s_context_test.IsEmpty())
            {
                _context = v8::Context::New(isolate);
                s_context_test.Reset(isolate, _context);
            }
            else
                _context = v8::Local<v8::Context>::New(isolate, s_context_test);

            v8::Context::Scope context_scope(_context);

            script = v8::Script::Compile(
                         v8::String::NewFromUtf8(isolate, src.c_str(),
                                                 v8::String::kNormalString, (int) src.length()),
                         v8::String::NewFromUtf8(isolate, name));
            if (script.IsEmpty())
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

        src = str + "){" + src + "\n});";

        script = v8::Script::Compile(
                     v8::String::NewFromUtf8(isolate, src.c_str(),
                                             v8::String::kNormalString, (int) src.length()),
                     v8::String::NewFromUtf8(isolate, name));
        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    v8::Local<v8::Object> _mod = v8::Object::New(isolate);

    _mod->Set(v8::String::NewFromUtf8(isolate, "_sbox"), m_sb->wrap());
    _mod->Set(v8::String::NewFromUtf8(isolate, "_id"), m_id);

    v8::Local<v8::Value> v = script->Run();
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(v);

    args[argCount - 2] = v8::Function::New(isolate, _require, _mod);
    args[argCount - 1] = v8::Function::New(isolate, _run, _mod);

    v = func->Call(v8::Object::New(isolate), argCount, args);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}

result_t SandBox::Context::run(std::string src, const char *name)
{
    static const char *names[] = {"require", "run"};
    v8::Local<v8::Value> args[2];

    return run(src, name, names, args, ARRAYSIZE(names));
}

result_t SandBox::Context::run(std::string src, const char *name, v8::Local<v8::Object> module,
                               v8::Local<v8::Object> exports)
{
    static const char *names[] = {"module", "exports", "require", "run"};
    v8::Local<v8::Value> args[4] = {module, exports};

    return run(src, name, names, args, ARRAYSIZE(names));
}

extern v8::Persistent<v8::Object> s_global;

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
        Context context(this, srcname);

        id.resize(id.length() - 3);

        v8::Local<v8::Object> mod;
        v8::Local<v8::Object> exports;

        // cache string
        v8::Local<v8::String> strRequire = v8::String::NewFromUtf8(isolate, "require");
        v8::Local<v8::String> strExports = v8::String::NewFromUtf8(isolate, "exports");

        exports = v8::Object::New(isolate);

        // module and exports object
        mod = v8::Object::New(isolate);

        // init module
        mod->Set(strExports, exports);
        mod->Set(strRequire, v8::Local<v8::Object>::New(isolate, s_global)->Get(strRequire));

        InstallModule(id, exports);

        std::string sname = name();
        if (!sname.empty())
        {
            sname.append(srcname);
            srcname = sname.c_str();
        }

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
        return CHECK_ERROR(Runtime::setError("Invalid file format."));

    return 0;
}

result_t SandBox::require(std::string base, std::string id,
                          v8::Local<v8::Value> &retVal, int32_t mode)
{
    std::string strId = resolvePath(base, id);
    std::string fname;
    std::map<std::string, VariantEx >::iterator it;

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

    retVal = _mods->Get(v8::String::NewFromUtf8(isolate, strId.c_str(),
                        v8::String::kNormalString,
                        (int)strId.length()));
    if (!IsEmpty(retVal))
        return 1;

    v8::Local<v8::Value> func = _mods->GetHiddenValue(v8::String::NewFromUtf8(isolate, "require"));
    if (!IsEmpty(func))
    {
        v8::Local<v8::Value> arg = v8::String::NewFromUtf8(isolate, strId.c_str());
        retVal = v8::Local<v8::Function>::Cast(func)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal))
        {
            if (retVal->IsObject() && !object_base::getInstance(retVal))
                retVal = retVal->ToObject()->Clone();
            InstallModule(strId, retVal);

            return 0;
        }
    }

    result_t hr;
    std::string buf;

    if (!fname.empty())
    {
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
            return addScript(fname.c_str(), buf.c_str(), retVal);
    }
    else
    {
        fname = strId + ".js";
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
            return addScript(fname.c_str(), buf.c_str(), retVal);

        fname = strId + ".json";
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
            return addScript(fname.c_str(), buf.c_str(), retVal);

        if (mode <= FILE_ONLY)
            return hr;

        fname = strId + PATH_SLASH + "package.json";
        hr = fs_base::ac_readFile(fname.c_str(), buf);
        if (hr >= 0)
        {
            v8::Local<v8::Value> v;
            hr = encoding_base::jsonDecode(buf.c_str(), v);
            if (hr < 0)
                return hr;

            v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
            v8::Local<v8::Value> main = o->Get(v8::String::NewFromUtf8(isolate, "main",
                                               v8::String::kNormalString, 4));
            if (!IsEmpty(main))
            {
                if (!main->IsString() && !main->IsStringObject())
                    return CHECK_ERROR(Runtime::setError("Invalid package.json"));
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

result_t SandBox::run(const char *fname)
{
    result_t hr;

    std::string sfname;
    path_base::normalize(fname, sfname);

    const char *pname = sfname.c_str();

    std::string buf;
    hr = fs_base::ac_readFile(pname, buf);
    if (hr < 0)
        return hr;

    if (buf[0] == '#' && buf[1] == '!')
        buf = "//" + buf;

    Context context(this, pname);

    std::string sname = name();
    if (!sname.empty())
    {
        sname.append(sfname);
        pname = sname.c_str();
    }

    return context.run(buf, pname);
}

}

