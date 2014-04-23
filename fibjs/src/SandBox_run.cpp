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
#include "ifs/console.h"

#include "ifs/Stat.h"
#include "ifs/global.h"
#include "ifs/Function.h"

#include <sstream>

namespace fibjs
{

inline const char *ToCString(const v8::String::Utf8Value &value)
{
    return *value ? *value : "<string conversion failed>";
}

inline result_t throwSyntaxError(v8::TryCatch &try_catch)
{
    v8::String::Utf8Value exception(try_catch.Exception());

    v8::Local<v8::Message> message = try_catch.Message();
    if (message.IsEmpty())
        ThrowError(ToCString(exception));
    else
    {
        std::stringstream strError;

        v8::String::Utf8Value filename(message->GetScriptResourceName());

        if (qstrcmp(ToCString(exception), "SyntaxError: ", 13))
            strError << ToCString(exception) << "\n    at ";
        else
            strError << (ToCString(exception) + 13) << "\n    at ";

        strError << ToCString(filename);
        int lineNumber = message->GetLineNumber();
        if (lineNumber > 0)
            strError << ':' << lineNumber << ':'
                     << (message->GetStartColumn() + 1);

        return Runtime::setError(strError.str());
    }

    return CALL_E_JAVASCRIPT;
}

inline std::string resolvePath(const char *id)
{
    std::string fname;

    if (id[0] == '.'
            && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
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
    v8::Local<v8::Context> context(v8::Context::New(isolate, NULL,
                                   fibjs::global_base::class_info().getTemplate()));
    v8::Context::Scope context_scope(context);
    result_t hr;

    v8::Local<v8::Object> glob = context->Global();
    v8::Local<v8::Object> mod;
    v8::Local<v8::Object> exports;

    glob->SetHiddenValue(v8::String::NewFromUtf8(isolate, "SandBox"), wrap());

    // cache string
    v8::Local<v8::String> strRequire = v8::String::NewFromUtf8(isolate, "require");
    v8::Local<v8::String> strExports = v8::String::NewFromUtf8(isolate, "exports");
    v8::Local<v8::String> strModule = v8::String::NewFromUtf8(isolate, "module");
    v8::Local<v8::String> strDefine = v8::String::NewFromUtf8(isolate, "define");
    v8::Local<v8::String> strId = v8::String::NewFromUtf8(isolate, "id");

    // attach define function first.
    v8::Local<v8::Function> def =
        v8::FunctionTemplate::New(isolate, _define)->GetFunction();

    def->ToObject()->Set(v8::String::NewFromUtf8(isolate, "amd"), v8::Object::New(isolate),
                         v8::ReadOnly);
    glob->Set(strDefine, def, v8::ReadOnly);

    // clone Function.start
    fibjs::Function_base::class_info().Attach(
        glob->Get(v8::String::NewFromUtf8(isolate, "Function"))->ToObject()->GetPrototype()->ToObject());

    // module.id
    std::string fname(srcname);
    fname.resize(fname.length() - 3);
    v8::Local<v8::String> strFname = v8::String::NewFromUtf8(isolate, fname.c_str(),
                                     v8::String::kNormalString,
                                     (int) fname.length());
    glob->SetHiddenValue(strId, strFname);

    exports = v8::Object::New(isolate);

    // module and exports object
    if (mod.IsEmpty())
        mod = v8::Object::New(isolate);

    // init module
    mod->Set(strExports, exports);
    mod->Set(strRequire, glob->Get(strRequire), v8::ReadOnly);

    mod->Set(strId, strFname, v8::ReadOnly);

    // add to modules
    InstallModule(fname, exports);

    // attach to global
    glob->Set(strModule, mod, v8::ReadOnly);
    glob->Set(strExports, exports, v8::ReadOnly);

    v8::Local<v8::Script> oscript;
    {
        v8::TryCatch try_catch;

        oscript = v8::Script::Compile(
                      v8::String::NewFromUtf8(isolate, script,
                                              v8::String::kNormalString, (int) qstrlen(script)),
                      v8::String::NewFromUtf8(isolate, srcname));
        if (oscript.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    if (oscript->Run().IsEmpty())
    {
        // delete from modules
        m_mods.erase(fname);
        return CALL_E_JAVASCRIPT;
    }

    // remove commonjs function
    glob->ForceDelete(strDefine);
    glob->ForceDelete(strModule);
    glob->ForceDelete(strExports);

    // process defined modules
    hr = doDefine(mod);
    if (hr < 0)
    {
        // delete from modules
        m_mods.erase(fname);
        return hr;
    }

    // attach again
    glob->Set(strModule, mod, v8::ReadOnly);
    glob->Set(strExports, exports, v8::ReadOnly);

    // use module.exports as result value
    v8::Local<v8::Value> v = mod->Get(strExports);
    if (!exports->Equals(v))
        InstallModule(fname, v);

    retVal = v;

    return 0;
}

result_t SandBox::require(const char *id, v8::Local<v8::Value> &retVal)
{
    std::string fname = resolvePath(id);
    std::map<std::string, VariantEx >::iterator it;

    // remove .js ext name if exists
    if (fname.length() > 3 && !qstrcmp(&fname[fname.length() - 3], ".js"))
        fname.resize(fname.length() - 3);

    it = m_mods.find(fname);

    if (it != m_mods.end())
    {
        retVal = it->second;
        return 1;
    }

    if (!m_require.IsEmpty())
    {
        v8::Local<v8::Value> arg = v8::String::NewFromUtf8(isolate, fname.c_str());
        retVal = v8::Local<v8::Function>::New(isolate, m_require)->Call(wrap(), 1, &arg);
        if (retVal.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (!IsEmpty(retVal))
        {
            if (retVal->IsObject())
                retVal = retVal->ToObject()->Clone();
            InstallModule(fname, retVal);

            return 0;
        }
    }

    // append .js ext name
    fname += ".js";

    result_t hr;
    const char *pname = fname.c_str();

    std::string buf;
    hr = fs_base::ac_readFile(pname, buf);
    if (hr < 0)
        return hr;

    return addScript(pname, buf.c_str(), retVal);
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

    v8::Local<v8::Context> context(v8::Context::New(isolate, NULL,
                                   fibjs::global_base::class_info().getTemplate()));
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> glob = context->Global();
    glob->SetHiddenValue(v8::String::NewFromUtf8(isolate, "SandBox"), wrap());

    // clone Function.start
    fibjs::Function_base::class_info().Attach(
        glob->Get(v8::String::NewFromUtf8(isolate, "Function"))->ToObject()->GetPrototype()->ToObject());

    // module.id
    sfname.resize(sfname.length() - 3);
    v8::Local<v8::String> strFname = v8::String::NewFromUtf8(isolate, sfname.c_str(),
                                     v8::String::kNormalString,
                                     (int) sfname.length());
    glob->SetHiddenValue(v8::String::NewFromUtf8(isolate, "id"), strFname);

    v8::Local<v8::Script> script;
    {
        v8::TryCatch try_catch;

        if (buf[0] == '#' && buf[1] == '!')
            buf = "//" + buf;

        script = v8::Script::Compile(
                     v8::String::NewFromUtf8(isolate, buf.c_str(),
                                             v8::String::kNormalString, (int) buf.length()),
                     v8::String::NewFromUtf8(isolate, pname));
        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    if (script->Run().IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}

}

