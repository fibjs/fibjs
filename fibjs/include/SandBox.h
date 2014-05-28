/*
 * SandBox.h
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "ifs/SandBox.h"
#include "ifs/global.h"
#include "ifs/Function.h"
#include <map>

#ifndef SANDBOX_H_
#define SANDBOX_H_

namespace fibjs
{

extern v8::Persistent<v8::Value> s_token;

class SandBox: public fibjs::SandBox_base
{
public:
    SandBox(const char *name = NULL)
    {
        if (name && *name)
        {
            m_name = name;
            m_name.append(":", 1);
        }
    }

    ~SandBox()
    {
        m_require.Reset();
    }

public:
    // SandBox_base
    virtual result_t add(const char *id, v8::Local<v8::Value> mod);
    virtual result_t add(v8::Local<v8::Object> mods);
    virtual result_t addScript(const char *srcname, const char *script, v8::Local<v8::Value> &retVal);
    virtual result_t remove(const char *id);
    virtual result_t run(const char *fname);
    virtual result_t require(const char *id, v8::Local<v8::Value> &retVal);

public:
    void initRoot();
    void initRequire(v8::Local<v8::Function> func)
    {
        m_require.Reset(isolate, func);
    }

    void InstallModule(std::string fname, v8::Local<v8::Value> o);
    inline void InstallNativeModule(const char *fname, ClassInfo &ci)
    {
        InstallModule(fname, ci.CreateInstance());
    }

    result_t repl();

public:
    class Context
    {
    public:
        Context(SandBox *sb, const char *id) :
            context(v8::Context::New(isolate))
        {
            v8::Local<v8::Context> ctx = isolate->GetCallingContext();
            static const char *s_skips_root[] =
            {
                "define", 0
            };
            static const char *s_skips[] =
            {
                "repl", "define", 0
            };

            context->SetSecurityToken(v8::Local<v8::Value>::New(isolate, s_token));
            context->Enter();

            glob = context->Global();
            global_base::class_info().Attach(glob, ctx.IsEmpty() ? s_skips_root : s_skips);

            glob->SetHiddenValue(v8::String::NewFromUtf8(isolate, "SandBox"), sb->wrap());

            // clone Function.start
            Function_base::class_info().Attach(
                glob->Get(v8::String::NewFromUtf8(isolate, "Function"))->ToObject()->GetPrototype()->ToObject(),
                NULL);

            // module.id
            v8::Local<v8::String> strFname = v8::String::NewFromUtf8(isolate, id,
                                             v8::String::kNormalString,
                                             (int) qstrlen(id));
            glob->SetHiddenValue(v8::String::NewFromUtf8(isolate, "id"), strFname);
        }

        ~Context()
        {
            context->Exit();
        }

        static result_t run(std::string &src, const char *name)
        {
            v8::Local<v8::Script> script;
            {
                v8::TryCatch try_catch;

                script = v8::Script::Compile(
                             v8::String::NewFromUtf8(isolate, src.c_str(),
                                                     v8::String::kNormalString, (int) src.length()),
                             v8::String::NewFromUtf8(isolate, name));
                if (script.IsEmpty())
                    return throwSyntaxError(try_catch);
            }

            if (script->Run().IsEmpty())
                return CALL_E_JAVASCRIPT;

            return 0;
        }

        static result_t run(const char *src, const char *name)
        {
            v8::Local<v8::Script> script;
            {
                v8::TryCatch try_catch;

                script = v8::Script::Compile(
                             v8::String::NewFromUtf8(isolate, src,
                                                     v8::String::kNormalString, (int) qstrlen(src)),
                             v8::String::NewFromUtf8(isolate, name));
                if (script.IsEmpty())
                    return throwSyntaxError(try_catch);
            }

            if (script->Run().IsEmpty())
                return CALL_E_JAVASCRIPT;

            return 0;
        }

        static result_t repl();

    public:
        v8::Local<v8::Context> context;
        v8::Local<v8::Object> glob;
    };

private:
    std::map<std::string, VariantEx > m_mods;
    v8::Persistent<v8::Function> m_require;
    std::string m_name;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
