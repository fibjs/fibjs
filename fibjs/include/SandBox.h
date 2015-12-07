/*
 * SandBox.h
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "ifs/SandBox.h"
#include "ifs/Stream.h"
#include "ifs/process.h"
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

        process_base::cwd(m_root);
    }

public:
    // SandBox_base
    virtual result_t add(const char *id, v8::Local<v8::Value> mod);
    virtual result_t add(v8::Local<v8::Object> mods);
    virtual result_t addScript(const char *srcname, const char *script, v8::Local<v8::Value> &retVal);
    virtual result_t remove(const char *id);
    virtual result_t clone(obj_ptr<SandBox_base> &retVal);
    virtual result_t run(const char *fname, v8::Local<v8::Array> argv);
    virtual result_t require(const char *id, v8::Local<v8::Value> &retVal);

public:
    v8::Local<v8::Object> mods()
    {
        Isolate* isolate = Isolate::now();

        const char *mods_name = "_mods";
        v8::Local<v8::Value> v = wrap()->GetHiddenValue(v8::String::NewFromUtf8(isolate->m_isolate, mods_name));
        v8::Local<v8::Object> o;

        if (!v.IsEmpty())
            o = v->ToObject();
        else
        {
            o = v8::Object::New(isolate->m_isolate);
            wrap()->SetHiddenValue(v8::String::NewFromUtf8(isolate->m_isolate, mods_name), o);
        }

        return o;
    }

    void initRoot();
    void initRequire(v8::Local<v8::Function> func)
    {
        mods()->SetHiddenValue(v8::String::NewFromUtf8(Isolate::now()->m_isolate, "require"), func);
    }

    void InstallModule(std::string fname, v8::Local<v8::Value> o);
    inline void InstallNativeModule(const char *fname, ClassInfo &ci)
    {
        InstallModule(fname, ci.getFunction());
    }

    result_t require(std::string base, std::string id, v8::Local<v8::Value> &retVal, int32_t mode);
    result_t repl(v8::Local<v8::Array> cmds, Stream_base* out = NULL);

    result_t run(const char *fname, v8::Local<v8::Array> argv, v8::Local<v8::Value> replFunc);

    std::string name()
    {
        return m_name;
    }

public:
    class Context
    {
    public:
        Context(SandBox *sb, const char *id);

        result_t run(std::string src, const char *name, const char **argNames,
                     v8::Local<v8::Value> *args, int32_t argCount);
        result_t run(std::string src, const char *name, v8::Local<v8::Array> argv, v8::Local<v8::Value> replFunc);
        result_t run(std::string src, const char *name, v8::Local<v8::Object> module,
                     v8::Local<v8::Object> exports);

        static result_t repl(v8::Local<v8::Array> cmds, Stream_base* out);

    public:
        obj_ptr<SandBox> m_sb;
        v8::Local<v8::Value> m_id;
        v8::Local<v8::Function> m_fnRequest;
        v8::Local<v8::Function> m_fnRun;
    };

    std::string m_name;
    std::string m_root;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
