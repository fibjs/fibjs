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
        Isolate* isolate = holder();

        v8::Local<v8::Value> v = isolate->GetPrivate(wrap(), "_mods");
        v8::Local<v8::Object> o;

        if (!v->IsUndefined())
            o = v->ToObject();
        else
        {
            o = v8::Object::New(isolate->m_isolate);
            isolate->SetPrivate(wrap(), "_mods", o);
        }

        return o;
    }

    void initRoot();
    void initRequire(v8::Local<v8::Function> func)
    {
        Isolate* isolate = holder();
        isolate->SetPrivate(mods(), "require", func);
    }

    void InstallModule(qstring fname, v8::Local<v8::Value> o);

    result_t require(qstring base, qstring id, v8::Local<v8::Value> &retVal, int32_t mode);
    result_t repl(v8::Local<v8::Array> cmds, Stream_base* out = NULL);

    result_t run(const char *fname, v8::Local<v8::Array> argv, bool main);

    qstring name()
    {
        return m_name;
    }

public:
    class Context
    {
    public:
        Context(SandBox *sb, const char *id);

        result_t run(qstring src, const char *name, const char **argNames,
                     v8::Local<v8::Value> *args, int32_t argCount);
        result_t run(qstring src, const char *name, v8::Local<v8::Array> argv, bool main);
        result_t run(qstring src, const char *name, v8::Local<v8::Object> module,
                     v8::Local<v8::Object> exports);

        static result_t repl(v8::Local<v8::Array> cmds, Stream_base* out);

    public:
        obj_ptr<SandBox> m_sb;
        v8::Local<v8::Value> m_id;
        v8::Local<v8::Function> m_fnRequest;
        v8::Local<v8::Function> m_fnRun;
    };

    qstring m_name;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
