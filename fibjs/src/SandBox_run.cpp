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

inline std::string resolvePath(const char *id)
{
    std::string fname;

    if (id[0] == '.' && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
    {
        v8::Local<v8::Context> ctx = isolate->GetCallingContext();

        if (!ctx.IsEmpty())
        {
            v8::Local<v8::Value> path = ctx->Global()->GetHiddenValue(
                                            v8::String::NewFromUtf8(isolate, "id"));

            if (!path.IsEmpty())
            {
                std::string strPath;

                path_base::dirname(*v8::String::Utf8Value(path), strPath);
                if (strPath.length())
                    strPath += '/';
                strPath += id;
                path_base::normalize(strPath.c_str(), fname);

                return fname;
            }
        }
    }

    path_base::normalize(id, fname);

    return fname;
}

void _define(const v8::FunctionCallbackInfo<v8::Value> &args);
result_t doDefine(v8::Local<v8::Object> &mod);

result_t SandBox::addScript(const char *srcname, const char *script,
                            v8::Local<v8::Value> &retVal)
{
    std::string fname(srcname);
    Context context(this, srcname);
    result_t hr;

    // add to modules
    std::string id(fname);

    if (id.length() > 5 && !qstrcmp(&id[id.length() - 5], ".json"))
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
    else if (id.length() > 3 && !qstrcmp(&id[id.length() - 3], ".js"))
    {
        id.resize(id.length() - 3);

        v8::Local<v8::Object> mod;
        v8::Local<v8::Object> exports;

        // cache string
        v8::Local<v8::String> strRequire = v8::String::NewFromUtf8(isolate, "require");
        v8::Local<v8::String> strExports = v8::String::NewFromUtf8(isolate, "exports");
        v8::Local<v8::String> strModule = v8::String::NewFromUtf8(isolate, "module");
        v8::Local<v8::String> strDefine = v8::String::NewFromUtf8(isolate, "define");

        // attach define function first.
        v8::Local<v8::Function> def =
            v8::FunctionTemplate::New(isolate, _define)->GetFunction();

        def->ToObject()->Set(v8::String::NewFromUtf8(isolate, "amd"), v8::Object::New(isolate),
                             v8::ReadOnly);
        context.glob->Set(strDefine, def, v8::ReadOnly);

        exports = v8::Object::New(isolate);

        // module and exports object
        mod = v8::Object::New(isolate);

        // init module
        mod->Set(strExports, exports);
        mod->Set(strRequire, context.glob->Get(strRequire), v8::ReadOnly);

        InstallModule(id, exports);

        // attach to global
        context.glob->Set(strModule, mod, v8::ReadOnly);
        context.glob->Set(strExports, exports, v8::ReadOnly);

        std::string sname;
        if (!m_name.empty())
        {
            sname = m_name + srcname;
            srcname = sname.c_str();
        }

        hr = context.run(script, srcname);
        if (hr < 0)
        {
            // delete from modules
            m_mods.erase(id);
            return hr;
        }

        // remove commonjs function
        context.glob->ForceDelete(strDefine);
        context.glob->ForceDelete(strModule);
        context.glob->ForceDelete(strExports);

        // process defined modules
        hr = doDefine(mod);
        if (hr < 0)
        {
            // delete from modules
            m_mods.erase(id);
            return hr;
        }

        // attach again
        context.glob->Set(strModule, mod, v8::ReadOnly);
        context.glob->Set(strExports, exports, v8::ReadOnly);

        // use module.exports as result value
        v8::Local<v8::Value> v = mod->Get(strExports);
        if (!exports->Equals(v))
            InstallModule(id, v);

        retVal = v;
        return 0;
    }

    return 0;
}

result_t SandBox::require(const char *id, v8::Local<v8::Value> &retVal)
{
    std::string stdId = resolvePath(id);
    std::map<std::string, VariantEx >::iterator it;

    it = m_mods.find(stdId);

    if (it != m_mods.end())
    {
        retVal = it->second;
        return 1;
    }

    if (!m_require.IsEmpty())
    {
        v8::Local<v8::Value> arg = v8::String::NewFromUtf8(isolate, stdId.c_str());
        retVal = v8::Local<v8::Function>::New(isolate, m_require)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal))
        {
            if (retVal->IsObject() && !object_base::getInstance(retVal))
                retVal = retVal->ToObject()->Clone();
            InstallModule(stdId, retVal);

            return 0;
        }
    }

    result_t hr;
    std::string buf;
    std::string fname;

    fname = stdId + ".js";
    hr = fs_base::ac_readFile(fname.c_str(), buf);
    if (hr >= 0)
        return addScript(fname.c_str(), buf.c_str(), retVal);

    fname = stdId + ".json";
    hr = fs_base::ac_readFile(fname.c_str(), buf);
    if (hr >= 0)
        return addScript(fname.c_str(), buf.c_str(), retVal);

    return hr;
}

result_t SandBox::run(const char *fname)
{
    result_t hr;

    std::string sfname = resolvePath(fname);
    const char *pname = sfname.c_str();

    std::string buf;
    hr = fs_base::ac_readFile(pname, buf);
    if (hr < 0)
        return hr;

    if (buf[0] == '#' && buf[1] == '!')
        buf = "//" + buf;

    Context context(this, pname);

    std::string sname;
    if (!m_name.empty())
    {
        sname = m_name + sfname;
        pname = sname.c_str();
    }

    return context.run(buf, pname);
}

}

