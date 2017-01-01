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
    SandBox(exlib::string name = "") : m_name(name)
    {}

public:
    // SandBox_base
    virtual result_t add(exlib::string id, v8::Local<v8::Value> mod);
    virtual result_t add(v8::Local<v8::Object> mods);
    virtual result_t compile(exlib::string srcname, exlib::string script, obj_ptr<Buffer_base>& retVal);
    virtual result_t compile(exlib::string script, obj_ptr<Buffer_base>& retVal);
    virtual result_t addScript(exlib::string srcname, Buffer_base* script, v8::Local<v8::Value>& retVal);
    virtual result_t remove(exlib::string id);
    virtual result_t clone(obj_ptr<SandBox_base> &retVal);
    virtual result_t run(exlib::string fname, v8::Local<v8::Array> argv);
    virtual result_t require(exlib::string id, exlib::string base, v8::Local<v8::Value> &retVal);

public:
    result_t addScript(exlib::string srcname, exlib::string script, v8::Local<v8::Value> &retVal);

    v8::Local<v8::Object> mods()
    {
        Isolate* isolate = holder();

        v8::Local<v8::Value> v = GetPrivate("_mods");
        v8::Local<v8::Object> o;

        if (!v->IsUndefined())
            o = v->ToObject();
        else
        {
            o = v8::Object::New(isolate->m_isolate);
            SetPrivate("_mods", o);
        }

        return o;
    }

    void initRoot();
    void initRequire(v8::Local<v8::Function> func)
    {
        SetPrivate("require", func);
    }

    void InstallModule(exlib::string fname, v8::Local<v8::Value> o);

    result_t require(exlib::string base, exlib::string id, v8::Local<v8::Value> &retVal, int32_t mode);
    result_t repl(v8::Local<v8::Array> cmds, Stream_base* out = NULL);

    result_t run(exlib::string fname, v8::Local<v8::Array> argv, bool main);

public:
    class Context
    {
    public:
        Context(SandBox *sb, exlib::string id);

        result_t run(Buffer_base* src, exlib::string name, v8::Local<v8::Value> *args);
        result_t run(exlib::string src, exlib::string name, v8::Local<v8::Value> *args);

        template<typename T>
        result_t run(T src, exlib::string name, v8::Local<v8::Array> argv, bool main);

        template<typename T>
        result_t run(T src, exlib::string name, v8::Local<v8::Object> module,
                     v8::Local<v8::Object> exports);

        static result_t repl(v8::Local<v8::Array> cmds, Stream_base* out);

    public:
        obj_ptr<SandBox> m_sb;
        v8::Local<v8::Value> m_id;
        v8::Local<v8::Function> m_fnRequest;
        v8::Local<v8::Function> m_fnRun;
    };

    exlib::string m_name;
};

} /* namespace fibjs */
#endif /* SANDBOX_H_ */
