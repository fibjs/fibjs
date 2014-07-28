/*
 * define.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/global.h"
#include "ifs/path.h"
#include "ifs/vm.h"
#include "SandBox.h"
#include <vector>
#include <set>
#include <map>
#include <sstream>

namespace fibjs
{

inline void InstallModule(std::string fname, v8::Local<v8::Value> o)
{
    static date_t s_noneDate;

    obj_ptr<SandBox_base> sbox;
    result_t hr = vm_base::current(sbox);
    if (hr < 0 || hr == CALL_RETURN_NULL)
        return;
    obj_ptr<SandBox> sbox1 = (SandBox *)(SandBox_base *)sbox;

    sbox1->InstallModule(fname, o);
}

inline std::string resolvePath(std::string base, const char *id)
{
    std::string fname;

    if (id[0] == '.'
            && (isPathSlash(id[1]) || (id[1] == '.' && isPathSlash(id[2]))))
    {
        if (base.length())
            base += '/';
        base += id;
        path_base::normalize(base.c_str(), fname);

        return fname;
    }

    path_base::normalize(id, fname);

    return fname;
}

void _define(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    int argc = args.Length();
    if (argc == 0)
    {
        ThrowResult(CALL_E_PARAMNOTOPTIONAL);
        return;
    }

    if (argc > 3)
    {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    //    v8::LocalScope handle_scope(isolate);

    v8::Local<v8::Object> glob = isolate->GetCallingContext()->Global();

    // cache string
    v8::Local<v8::String> strRequire = v8::String::NewFromUtf8(isolate, "require");
    v8::Local<v8::String> strExports = v8::String::NewFromUtf8(isolate, "exports");
    v8::Local<v8::String> strModule = v8::String::NewFromUtf8(isolate, "module");
    v8::Local<v8::String> strDefs = v8::String::NewFromUtf8(isolate, "defs");
    v8::Local<v8::String> strId = v8::String::NewFromUtf8(isolate, "id");

    // fetch default module object
    v8::Local<v8::Object> mod = glob->Get(strModule)->ToObject();

    // fetch module.id
    std::string path;
    path_base::dirname(*v8::String::Utf8Value(mod->Get(strId)), path);

    v8::Local<v8::Array> defs;
    {
        v8::Local<v8::Value> v;

        // fetch hidden value module.defs
        v = mod->GetHiddenValue(strDefs);
        if (!v.IsEmpty() && v->IsArray())
            defs = v8::Local<v8::Array>::Cast(v);
        else
        {
            // create one if not exists.
            defs = v8::Array::New(isolate);
            mod->SetHiddenValue(strDefs, defs);
        }
    }

    v8::Local<v8::Array> deps;
    std::string id;

    int n = argc - 1, i;

    // check deps array
    if ((n > 0) && (args[n - 1]->IsArray()))
    {
        v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(args[n - 1]);
        int an = a->Length();

        deps = v8::Array::New(isolate, an);

        // copy deps array to module.deps
        for (i = 0; i < an; i++)
        {
            v8::String::Utf8Value s(a->Get(i));
            deps->Set(i, v8::String::NewFromUtf8(isolate, resolvePath(path, *s).c_str()));
        }
        n--;
    }
    else
    {
        deps = v8::Array::New(isolate, 3);

        // default deps: ['require', 'exports', 'module']
        deps->Set(0, strRequire);
        deps->Set(1, strExports);
        deps->Set(2, strModule);
    }

    if (n > 1)
    {
        ThrowResult(CALL_E_INVALIDARG);
        return;
    }
    else if (n == 1)
    {
        // we have an id name
        v8::String::Utf8Value s(args[0]);
        id = *s;
    }

    v8::Local<v8::Object> modDef;

    if (id.empty())
    {
        // anonymous module attach default module
        modDef = mod;
    }
    else
    {
        // named module
        id = resolvePath(path, id.c_str());

        modDef = v8::Object::New(isolate);
        v8::Local<v8::Object> exports = v8::Object::New(isolate);

        // init module properties
        modDef->Set(strExports, exports);
        modDef->ForceSet(strRequire, glob->Get(strRequire), v8::ReadOnly);

        v8::Local<v8::String> strFname = v8::String::NewFromUtf8(isolate, id.c_str(),
                                         v8::String::kNormalString, (int) id.length());
        modDef->ForceSet(strId, strFname, v8::ReadOnly);

        // add to modules
        InstallModule(id, exports);
    }

    v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(isolate, 1,
                                           v8::StackTrace::kOverview);
    std::stringstream strBuffer;

    v8::Local<v8::StackFrame> f = stackTrace->GetFrame(0);

    v8::String::Utf8Value funname(f->GetFunctionName());
    v8::String::Utf8Value filename(f->GetScriptName());

    strBuffer << "\n    at ";

    if (**funname)
        strBuffer << *funname << " (";

    if (*filename)
        strBuffer << *filename << ':' << f->GetLineNumber() << ':'
                  << f->GetColumn();
    else
        strBuffer << "[eval]:" << f->GetLineNumber() << ':'
                  << f->GetColumn();

    if (**funname)
        strBuffer << ')';
    std::string strStack = strBuffer.str();

    // set hidden value module.deps and module.factory
    modDef->SetHiddenValue(v8::String::NewFromUtf8(isolate, "deps"), deps);
    modDef->SetHiddenValue(v8::String::NewFromUtf8(isolate, "factory"), args[argc - 1]);
    modDef->SetHiddenValue(v8::String::NewFromUtf8(isolate, "stack"),
                           v8::String::NewFromUtf8(isolate, strStack.c_str(),
                                   v8::String::kNormalString, (int) strStack.length()));

    // append to define array
    defs->Set(defs->Length(), modDef);

    args.GetReturnValue().Set(v8::Null(isolate));
}

result_t doDefine(v8::Local<v8::Object> &mod)
{
    v8::Local<v8::Array> defs;
    {
        v8::Local<v8::Value> v;
        v8::Local<v8::String> strDefs = v8::String::NewFromUtf8(isolate, "defs");

        // get define array from default module
        v = mod->GetHiddenValue(strDefs);
        mod->DeleteHiddenValue(strDefs);

        if (!v.IsEmpty() && v->IsArray())
            defs = v8::Local<v8::Array>::Cast(v);
        else
            return 0;
    }

    int an = defs->Length(), i, j;
    std::vector<v8::Local<v8::Object> > mods;
    std::vector<std::string> modIds;
    std::set<std::string> depns;

    // cache string
    v8::Local<v8::String> strRequire = v8::String::NewFromUtf8(isolate, "require");
    v8::Local<v8::String> strExports = v8::String::NewFromUtf8(isolate, "exports");
    v8::Local<v8::String> strDeps = v8::String::NewFromUtf8(isolate, "deps");
    v8::Local<v8::String> strFactory = v8::String::NewFromUtf8(isolate, "factory");
    v8::Local<v8::String> strId = v8::String::NewFromUtf8(isolate, "id");

    // copy data to local
    mods.resize(an);
    modIds.resize(an);
    for (i = 0; i < an; i++)
    {
        mods[i] = defs->Get(i)->ToObject();
        modIds[i] = *v8::String::Utf8Value(mods[i]->Get(strId));
        depns.insert(modIds[i]);
    }

    // two step
    int doStep = 2;
    bool bNext = false;

    while (doStep)
    {
        bNext = true;

        for (i = 0; i < an; i++)
        {
            if (!modIds[i].empty())
            {
                // get deps array
                v8::Local<v8::Value> a1 = mods[i]->GetHiddenValue(strDeps);

                if (a1.IsEmpty() || !a1->IsArray())
                {
                    // not found deps array
                    depns.erase(modIds[i]);
                    modIds[i].clear();
                    bNext = false;

                    continue;
                }

                v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(a1);

                int n = a->Length();

                // check if the module depend a module defined in same script
                for (j = 0; j < n; j++)
                    if (doStep == 2
                            && depns.find(*v8::String::Utf8Value(a->Get(j)))
                            != depns.end())
                        break;

                if (j == n)
                {
                    std::vector<v8::Local<v8::Value> > deps;
                    deps.resize(n);

                    for (j = 0; j < n; j++)
                    {
                        v8::String::Utf8Value id(a->Get(j));

                        if (!qstrcmp(*id, "require"))
                            deps[j] = mods[i]->Get(strRequire);
                        else if (!qstrcmp(*id, "exports"))
                            deps[j] = mods[i]->Get(strExports);
                        else if (!qstrcmp(*id, "module"))
                            deps[j] = mods[i];
                        else
                        {
                            // load module use require
                            result_t hr = global_base::require(*id, deps[j]);
                            if (hr < 0)
                                return hr;
                        }
                    }

                    v8::Local<v8::Value> v;

                    // get factory and remove hidden value.
                    v = mods[i]->GetHiddenValue(strFactory);
                    mods[i]->DeleteHiddenValue(strFactory);
                    mods[i]->DeleteHiddenValue(strDeps);

                    if (v->IsFunction())
                    {
                        v8::Local<v8::Function> func =
                            v8::Local<v8::Function>::Cast(v);

                        v = func->Call(func, n, deps.data());
                        if (v.IsEmpty())
                            return CHECK_ERROR(CALL_E_JAVASCRIPT);
                    }

                    // use the result as exports if the factory return something
                    if (!IsEmpty(v))
                        mods[i]->Set(strExports, v);
                    else
                        v = mods[i]->Get(strExports);

                    InstallModule(modIds[i], v);

                    // remove id name, we don't like to call it again
                    depns.erase(modIds[i]);
                    modIds[i].clear();
                    bNext = false;
                }
            }
        }

        if (bNext)
            doStep--;
    }

    return 0;
}

result_t global_base::define(const char *id, v8::Local<v8::Array> deps,
                             v8::Local<v8::Value> factory)
{
    return 0;
}

}
