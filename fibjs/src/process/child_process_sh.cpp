/*
 * child_process_sh.cpp
 *
 *  Created on: Apr 30, 2025
 *      Author: lion
 */

#include "object.h"
#undef stdout
#undef stderr
#include "ifs/child_process.h"
#include "StringBuffer.h"

namespace fibjs {

class AsyncShell : public AsyncEvent {
public:
    AsyncShell(exlib::string& retVal, AsyncEvent* ac)
        : m_retVal(retVal)
        , m_ac(ac)
    {
        m_ctx.resize(1);
        m_ctx[0] = ac->m_ctx[0];
        m_ctxo = ac->m_ctxo;

        setAsync();
    }

    static exlib::string process_output(Variant& v)
    {
        exlib::string s = v.string();
        const char* c_s = s.c_str();
        size_t sz = s.length();

#ifdef _WIN32
        if (sz > 1 && c_s[sz - 2] == '\r' && c_s[sz - 1] == '\n')
            s.resize(sz - 2);
#else
        if (sz > 0 && c_s[sz - 1] == '\n')
            s.resize(sz - 1);
#endif

        return s;
    }

    virtual int32_t post(int32_t v)
    {
        if (m_exec_retVal->exitCode) {
            m_ac->post(Runtime::setError(process_output(m_exec_retVal->stderr)));
        } else {
            m_retVal = process_output(m_exec_retVal->stdout);
            m_ac->post(v);
        }
        delete this;
        return 0;
    }

    static exlib::string process_command(v8::Local<v8::Array> strings, OptArgs args)
    {
        Isolate* isolate = Isolate::current(strings);
        v8::Local<v8::Context> context = isolate->context();
        StringBuffer sbuf;
        int32_t argc = strings->Length();
        size_t strcnt = strings->Length();

        for (int32_t i = 0; i < strcnt; i++) {
            v8::Local<v8::Value> v = strings->Get(context, i).FromMaybe(v8::Local<v8::Value>());
            v8::String::Utf8Value str(isolate->m_isolate, v);
            sbuf.append(*str, str.length());

            if (i < argc - 1) {
                v8::Local<v8::Value> v1 = args[i];

                if (v1->IsArray()) {
                    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v1);
                    int32_t len = arr->Length();

                    for (int32_t j = 0; j < len; j++) {
                        v8::Local<v8::Value> v2 = arr->Get(context, j).FromMaybe(v8::Local<v8::Value>());
                        v8::String::Utf8Value str2(isolate->m_isolate, v2);
                        if (*str2)
                            sbuf.append(*str2, str2.length());

                        if (j < len - 1)
                            sbuf.append(' ');
                    }
                } else {
                    v8::String::Utf8Value str1(isolate->m_isolate, v1);
                    if (*str1)
                        sbuf.append(*str1, str1.length());
                }
            }
        }

        return sbuf.str();
    }

public:
    obj_ptr<child_process_base::ExecType> m_exec_retVal;

private:
    exlib::string& m_retVal;
    AsyncEvent* m_ac;
};

result_t child_process_base::sh(v8::Local<v8::Array> strings, OptArgs args, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(strings);
        exlib::string cmd = AsyncShell::process_command(strings, args);

        ac->m_ctx.resize(2);
        ac->m_ctx[1] = cmd;

        return exec(cmd, v8::Local<v8::Object>(), *(obj_ptr<ExecType>*)nullptr, ac);
    }

    AsyncShell* as = new AsyncShell(retVal, ac);
    exlib::string cmd = ac->m_ctx[1].string();

    return exec(cmd, v8::Local<v8::Object>(), as->m_exec_retVal, as);
}

static void js_ssh(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Array> _args = args.Data().As<v8::Array>();

    v8::Local<v8::Array> strings;
    result_t hr = GetArgumentValue(isolate, args[0], strings, true);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    OptArgs optargs(args, 1, args.Length() - 1);

    exlib::string command = AsyncShell::process_command(strings, optargs);
    _args->Set(isolate->context(), 3, isolate->NewString(command)).IsJust();

    obj_ptr<child_process_base::ExecFileType> retVal;
    hr = child_process_base::ac_execFile("ssh", _args, v8::Local<v8::Object>(), retVal);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    if (retVal->exitCode) {
        ThrowResult(Runtime::setError(AsyncShell::process_output(retVal->stderr)));
    } else {
        args.GetReturnValue().Set(isolate->NewString(AsyncShell::process_output(retVal->stdout)));
    }
}

result_t child_process_base::ssh(exlib::string host, v8::Local<v8::Object> options, v8::Local<v8::Function>& retVal)
{
    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, 4);

    exlib::string user;
    GetConfigValue(isolate, options, "user", user);
    if (!user.empty())
        host = user + "@" + host;

    int32_t port = 22;
    GetConfigValue(isolate, options, "port", port);
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    args->Set(context, 0, isolate->NewString(host)).IsJust();
    args->Set(context, 1, isolate->NewString("-p")).IsJust();
    args->Set(context, 2, isolate->NewString(port_str)).IsJust();

    retVal = isolate->NewFunction("ssh", js_ssh, args);
    return 0;
}

}