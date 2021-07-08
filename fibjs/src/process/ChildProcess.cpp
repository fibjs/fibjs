/*
 * ChildProcess.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "ifs/util.h"
#include "ChildProcess.h"
#include "UVStream.h"

namespace fibjs {

void ChildProcess::on_uv_close(uv_handle_t* handle)
{
    ChildProcess* cp = container_of(handle, ChildProcess, m_process);

    cp->isolate_unref();
    cp->m_vholder.Release();
}

void ChildProcess::OnExit(uv_process_t* handle, int64_t exit_status, int term_signal)
{
    ChildProcess* cp = container_of(handle, ChildProcess, m_process);
    Variant args[2];

    cp->m_exitCode = (int32_t)exit_status;
    cp->m_ev.set();

    args[0] = (double)exit_status;
    if (term_signal)
        args[1] = signo_string(term_signal);
    else
        args[1].setNull();

    cp->_emit("exit", args, 2);
    uv_close((uv_handle_t*)handle, on_uv_close);
}

result_t ChildProcess::fill_stdio(v8::Local<v8::Object> options)
{
    result_t hr;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    int32_t i;

    Variant stddefs[3];
    v8::Local<v8::Value> v;
    hr = GetConfigValue(isolate->m_isolate, options, "stdio", v);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        for (i = 0; i < 3; i++)
            stddefs[i] = "pipe";
    } else {
        exlib::string s;
        hr = GetArgumentValue(isolate->m_isolate, v, s, true);
        if (hr >= 0) {
            for (i = 0; i < 3; i++)
                stddefs[i] = s;
        } else {
            v8::Local<v8::Array> a;
            hr = GetArgumentValue(isolate->m_isolate, v, a, true);
            if (hr >= 0) {
                for (i = 0; i < 3; i++)
                    stddefs[i] = JSValue(a->Get(context, i));
            } else {
                for (i = 0; i < 3; i++)
                    stddefs[i] = "pipe";
            }
        }
    }

    uv_options.stdio = stdios;
    uv_options.stdio_count = 3;

#ifndef _WIN32
    int32_t pty_cnt = 0;
    for (i = 0; i < 3; i++)
        if (stddefs[i].type() == Variant::VT_String && stddefs[i].string() == "pty")
            pty_cnt++;

    if (pty_cnt == 3) {
        m_pty = true;

        for (i = 0; i < 3; i++) {
            stdios[i].flags = UV_INHERIT_FD;
            stdios[i].data.fd = i;
        }

        return 0;
    }

    if (pty_cnt > 0)
        return CHECK_ERROR(Runtime::setError("ChildProcess: every element of stdio must be \'pty\'."));
#endif

    for (i = 0; i < 3; i++) {
        if (stddefs[i].type() == Variant::VT_Integer) {
            stdios[i].flags = UV_INHERIT_FD;
            stdios[i].data.fd = stddefs[i].intVal();
        } else {
            exlib::string s = stddefs[i].type() != Variant::VT_String ? "pipe" : stddefs[i].string();

            if (s == "ignore") {
                stdios[i].flags = UV_IGNORE;
            } else if (s == "pipe") {
                hr = UVStream::create_pipe(m_stdio[i]);
                if (hr < 0)
                    return hr;

                stdios[i].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
                stdios[i].data.stream = (uv_stream_t*)&m_stdio[i]->m_pipe;
            } else {
                stdios[i].flags = UV_INHERIT_FD;
                stdios[i].data.fd = i;
            }
        }
    }

    return 0;
}

result_t ChildProcess::fill_env(v8::Local<v8::Object> options)
{
    result_t hr;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    int32_t len, i;

    int32_t uid;
    hr = GetConfigValue(isolate->m_isolate, options, "uid", uid);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;

        uv_options.flags |= UV_PROCESS_SETUID;
        uv_options.uid = (uv_uid_t)uid;
    }

    int32_t gid;
    hr = GetConfigValue(isolate->m_isolate, options, "gid", gid);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;

        uv_options.flags |= UV_PROCESS_SETGID;
        uv_options.gid = (uv_gid_t)gid;
    }

    v8::Local<v8::Object> opt_envs;
    hr = GetConfigValue(isolate->m_isolate, options, "env", opt_envs, true);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        hr = process_base::get_env(opt_envs);
        if (hr < 0)
            return hr;
    } else if (hr < 0)
        return hr;

    JSArray keys = opt_envs->GetPropertyNames(opt_envs->CreationContext());
    len = (int32_t)keys->Length();

    envStr.resize(len);
    _envs.resize(len + 1);

    for (i = 0; i < len; i++) {
        JSValue k = keys->Get(context, i);
        JSValue v = opt_envs->Get(context, k);
        exlib::string vs;
        exlib::string& ks = envStr[i];

        hr = GetArgumentValue(k, ks);
        if (hr < 0)
            return hr;

        if (!IsEmpty(v)) {
            hr = GetArgumentValue(v, vs);
            if (hr < 0)
                return hr;
        } else
            vs = exlib::string("");

        ks.append(1, '=');
        ks.append(vs);
        ks.append(1, 0);

        _envs[i] = (char*)envStr[i].c_str();
    }

    _envs[i] = NULL;
    uv_options.env = _envs.data();

    return 0;
}

result_t ChildProcess::fill_arg(exlib::string command, v8::Local<v8::Array> args)
{
    result_t hr;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    int32_t len, i;

    len = args.IsEmpty() ? 0 : args->Length();
    argStr.resize(len);
    _args.resize(len + 2);

    _args[0] = (char*)command.c_str();
    for (i = 0; i < len; i++) {
        hr = GetArgumentValue(isolate->m_isolate, JSValue(args->Get(context, i)), argStr[i]);
        if (hr < 0)
            return hr;

        _args[i + 1] = (char*)argStr[i].c_str();
    }
    _args[i + 1] = NULL;

    uv_options.args = _args.data();
    uv_options.file = command.c_str();

    return 0;
}

result_t ChildProcess::fill_opt(v8::Local<v8::Object> options)
{
    Isolate* isolate = Isolate::current();

    process_base::cwd(cwd);
    GetConfigValue(isolate->m_isolate, options, "cwd", cwd);
    uv_options.cwd = cwd.c_str();

    bool detached = false;
    GetConfigValue(isolate->m_isolate, options, "detached", detached);
    if (detached)
        uv_options.flags |= UV_PROCESS_DETACHED;

    bool windowsVerbatimArguments = false;
    GetConfigValue(isolate->m_isolate, options, "windowsVerbatimArguments", windowsVerbatimArguments);
    if (windowsVerbatimArguments)
        uv_options.flags |= UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS;

    bool windowsHide = false;
    GetConfigValue(isolate->m_isolate, options, "windowsHide", windowsHide);
    if (windowsHide)
        uv_options.flags |= UV_PROCESS_WINDOWS_HIDE;

    return 0;
}

extern "C" int pty_spawn(uv_loop_t* loop, uv_process_t* process, const uv_process_options_t* options, int* terminalfd);
result_t ChildProcess::spawn(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options)
{
    result_t hr;
    Isolate* isolate = Isolate::current();

    hr = fill_stdio(options);
    if (hr < 0)
        return hr;

    hr = fill_env(options);
    if (hr < 0)
        return hr;

    hr = fill_arg(command, args);
    if (hr < 0)
        return hr;

    hr = fill_opt(options);
    if (hr < 0)
        return hr;

    isolate_ref();
    m_vholder = new ValueHolder(wrap());

    return uv_call([&] {
        int32_t err;

#ifndef _WIN32
        if (m_pty) {
            int32_t terminalfd;
            err = pty_spawn(s_uv_loop, &m_process, &uv_options, &terminalfd);
            if (err >= 0) {
                UVStream::uv_pipe(m_stdio[0], terminalfd);
                m_stdio[1] = m_stdio[0];
            }
        } else
#endif
            err = uv_spawn(s_uv_loop, &m_process, &uv_options);

        if (err < 0)
            uv_close((uv_handle_t*)&m_process, on_uv_close);
        return err;
    });
}

result_t ChildProcess::kill(int32_t signal)
{
    return uv_process_kill(&m_process, signal);
}

result_t ChildProcess::join(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);
    return m_ev.wait(ac) ? 0 : CALL_E_PENDDING;
}

result_t ChildProcess::get_pid(int32_t& retVal)
{
    retVal = m_process.pid;
    return 0;
}

result_t ChildProcess::get_exitCode(int32_t& retVal)
{
    if (!m_ev.isSet())
        return CALL_E_INVALID_CALL;

    retVal = m_exitCode;
    return 0;
}

result_t ChildProcess::get_stdin(obj_ptr<Stream_base>& retVal)
{
    if (m_stdio[0] == NULL)
        return CHECK_ERROR(CALL_RETURN_NULL);

    retVal = m_stdio[0];

    return 0;
}

result_t ChildProcess::get_stdout(obj_ptr<Stream_base>& retVal)
{
    if (m_stdio[1] == NULL)
        return CHECK_ERROR(CALL_RETURN_NULL);

    retVal = m_stdio[1];

    return 0;
}

result_t ChildProcess::get_stderr(obj_ptr<Stream_base>& retVal)
{
    if (m_stdio[2] == NULL)
        return CHECK_ERROR(CALL_RETURN_NULL);

    retVal = m_stdio[2];

    return 0;
}
}