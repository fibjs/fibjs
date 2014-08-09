/*
 * SandBox.h
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "ifs/SandBox.h"
#include <map>

#ifndef SANDBOX_H_
#define SANDBOX_H_

namespace fibjs
{

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

public:
    // SandBox_base
    virtual result_t add(const char *id, v8::Local<v8::Value> mod);
    virtual result_t add(v8::Local<v8::Object> mods);
    virtual result_t addScript(const char *srcname, const char *script, v8::Local<v8::Value> &retVal);
    virtual result_t remove(const char *id);
    virtual result_t run(const char *fname);
    virtual result_t require(const char *id, v8::Local<v8::Value> &retVal);

public:
    v8::Local<v8::Object> mods()
    {
        const char *mods_name = "_mods";
        v8::Local<v8::Value> v = wrap()->GetHiddenValue(v8::String::NewFromUtf8(isolate, mods_name));
        v8::Local<v8::Object> o;

        if (!v.IsEmpty())
            o = v->ToObject();
        else
        {
            o = v8::Object::New(isolate);
            wrap()->SetHiddenValue(v8::String::NewFromUtf8(isolate, mods_name), o);
        }

        return o;
    }

    void initRoot();
    void initRequire(v8::Local<v8::Function> func)
    {
        mods()->SetHiddenValue(v8::String::NewFromUtf8(isolate, "require"), func);
    }

    void InstallModule(std::string fname, v8::Local<v8::Value> o);
    inline void InstallNativeModule(const char *fname, ClassInfo &ci)
    {
        InstallModule(fname, ci.CreateInstance());
    }

    result_t repl();

    result_t require(const char *id, v8::Local<v8::Value> &retVal, int32_t mode);


    std::string name()
    {
        return m_name;
    }

public:
    class ScriptContext : public object_base
    {
    public:
        ScriptContext(int32_t sid);
        ~ScriptContext();

        static v8::Local<v8::Object> GetCallingContext();

    private:
        int32_t m_sid;
    };

    class Context
    {
    public:
        Context(SandBox *sb, const char *id) : m_sb(sb)
        {
            m_id = v8::String::NewFromUtf8(isolate, id, v8::String::kNormalString,
                                           (int) qstrlen(id));
        }

        result_t run(std::string src, const char *name, const char **argNames = NULL,
                     v8::Local<v8::Value> *args = NULL, int32_t argCount = 0)
        {
            v8::Local<v8::Script> script;
            {
                v8::TryCatch try_catch;
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

            obj_ptr<ScriptContext> ctx = new ScriptContext(script->GetUnboundScript()->GetId());
            v8::Local<v8::Object> _mod = ctx->wrap();

            _mod->SetHiddenValue(v8::String::NewFromUtf8(isolate, "_sbox"), m_sb->wrap());
            _mod->SetHiddenValue(v8::String::NewFromUtf8(isolate, "_id"), m_id);

            v8::Local<v8::Value> v = script->Run();
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(v);
            func->SetHiddenValue(v8::String::NewFromUtf8(isolate, "_ctx"), _mod);

            v = func->Call(v8::Object::New(isolate), argCount, args);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            return 0;
        }

        static result_t repl();

    public:
        obj_ptr<SandBox> m_sb;
        v8::Local<v8::Value> m_id;
    };

    std::string m_name;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
