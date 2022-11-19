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

result_t ChildProcess::create_pipe(int32_t idx)
{
    result_t hr = UVStream::create_pipe(m_stdio[idx], m_ipc == idx);
    if (hr < 0)
        return hr;

    stdios[idx].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
    stdios[idx].data.stream = (uv_stream_t*)&m_stdio[idx]->m_pipe;

    if (idx == 3)
        uv_options.stdio_count = 4;

    return 0;
}

result_t ChildProcess::fill_stdio(v8::Local<v8::Object> options, bool fork)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    int32_t i;

    Variant stddefs[3];
    v8::Local<v8::Value> v;
    hr = GetConfigValue(isolate->m_isolate, options, "stdio", v);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        for (i = 0; i < 3; i++)
            stddefs[i] = fork ? "inherit" : "pipe";
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

        if (fork) {
            m_ipc = 3;
            return create_pipe(3);
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
                hr = create_pipe(i);
                if (hr < 0)
                    return hr;
            } else if (s == "ipc") {
                if (m_ipc >= 0)
                    return CHECK_ERROR(Runtime::setError("ChildProcess: Child process can have only one IPC pipe."));

                m_ipc = i;
                hr = create_pipe(i);
                if (hr < 0)
                    return hr;
            } else {
                stdios[i].flags = UV_INHERIT_FD;
                stdios[i].data.fd = i;
            }
        }
    }

    if (fork && m_ipc < 0) {
        m_ipc = 3;
        return create_pipe(3);
    }

    return 0;
}

result_t ChildProcess::fill_env(v8::Local<v8::Object> options)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

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
    int32_t len, sz, idx;

    sz = len = (int32_t)keys->Length();

    if (m_ipc >= 0)
        sz++;

    envStr.resize(sz);
    _envs.resize(sz + 1);

    int32_t p = 0;
    for (idx = 0; idx < len; idx++) {
        JSValue k = keys->Get(context, idx);
        JSValue v = opt_envs->Get(context, k);
        exlib::string vs;
        exlib::string& ks = envStr[p];

        hr = GetArgumentValue(k, ks);
        if (hr < 0)
            return hr;

        if (ks == "NODE_CHANNEL_FD")
            continue;

        if (!IsEmpty(v)) {
            hr = GetArgumentValue(v, vs);
            if (hr < 0)
                return hr;
        } else
            vs = exlib::string("");

        ks.append(1, '=');
        ks.append(vs);
        ks.append(1, 0);

        _envs[p] = (char*)envStr[p].c_str();
        p++;
    }

    if (m_ipc >= 0) {
        exlib::string& ks = envStr[p];
        exlib::string v;

        ks = "NODE_CHANNEL_FD";
        ks.append(1, '=');
        ks.append(1, '0' + m_ipc);
        ks.append(1, 0);

        _envs[p] = (char*)envStr[p].c_str();
        p++;
    }

    _envs[p] = NULL;
    uv_options.env = _envs.data();

    return 0;
}

result_t ChildProcess::fill_arg(exlib::string command, v8::Local<v8::Array> args)
{
    result_t hr;
    Isolate* isolate = holder();
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
    Isolate* isolate = holder();

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
result_t ChildProcess::spawn(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, bool fork)
{
    result_t hr;
    Isolate* isolate = holder();

    hr = fill_stdio(options, fork);
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

    hr = uv_call([&] {
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
    if (hr < 0)
        return hr;

    if (m_ipc >= 0) {
        if (m_ipc != 3) {
            m_stdio[3] = m_stdio[m_ipc];
            m_stdio[m_ipc].Release();
        }

        m_channel = m_stdio[3];
        new Ipc(isolate, wrap(), m_channel);
    }

    return hr;
}

result_t ChildProcess::kill(int32_t signal)
{
    return uv_process_kill(&m_process, signal);
}

result_t ChildProcess::kill(exlib::string signal)
{
    int32_t signo;

    if (signal == "SIGHUP")
        signo = 1;
    else if (signal == "SIGINT")
        signo = 2;
    else if (signal == "SIGQUIT")
        signo = 3;
    else if (signal == "SIGILL")
        signo = 4;
    else if (signal == "SIGTRAP")
        signo = 5;
    else if (signal == "SIGABRT")
        signo = 6;
    else if (signal == "SIGIOT")
        signo = 6;
    else if (signal == "SIGFPE")
        signo = 8;
    else if (signal == "SIGKILL")
        signo = 9;
    else if (signal == "SIGBUS")
        signo = 10;
    else if (signal == "SIGSEGV")
        signo = 11;
    else if (signal == "SIGSYS")
        signo = 12;
    else if (signal == "SIGPIPE")
        signo = 13;
    else if (signal == "SIGALRM")
        signo = 14;
    else if (signal == "SIGTERM")
        signo = 15;
    else if (signal == "SIGURG")
        signo = 16;
    else if (signal == "SIGSTOP")
        signo = 17;
    else if (signal == "SIGTSTP")
        signo = 18;
    else if (signal == "SIGCONT")
        signo = 19;
    else if (signal == "SIGCHLD")
        signo = 20;
    else if (signal == "SIGTTIN")
        signo = 21;
    else if (signal == "SIGTTOU")
        signo = 22;
    else if (signal == "SIGIO")
        signo = 23;
    else if (signal == "SIGXCPU")
        signo = 24;
    else if (signal == "SIGXFSZ")
        signo = 25;
    else if (signal == "SIGVTALRM")
        signo = 26;
    else if (signal == "SIGPROF")
        signo = 27;
    else if (signal == "SIGWINCH")
        signo = 28;
    else if (signal == "SIGINFO")
        signo = 29;
    else if (signal == "SIGUSR1")
        signo = 30;
    else if (signal == "SIGUSR2")
        signo = 31;
    else
        signo = 0;

    return kill(signo);
}

result_t ChildProcess::join(int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    class WaitExitCode : public AsyncEvent {
    public:
        WaitExitCode(ChildProcess_base* cp, int32_t& retVal, AsyncEvent* ac)
            : m_this(cp)
            , m_retVal(retVal)
            , m_ac(ac)
        {
            setAsync();
        }

        virtual int32_t post(int32_t v)
        {
            m_this->get_exitCode(m_retVal);

            m_ac->post(v);
            delete this;

            return 0;
        }

    private:
        obj_ptr<ChildProcess_base> m_this;
        int32_t& m_retVal;
        AsyncEvent* m_ac;
    };

    AsyncEvent* _ac = new WaitExitCode(this, retVal, ac);
    if (m_ev.wait(_ac)) {
        retVal = m_exitCode;
        delete _ac;
        return 0;
    }

    return CALL_E_PENDDING;
}

result_t ChildProcess::get_connected(bool& retVal)
{
    retVal = !!m_channel;
    return 0;
}

result_t ChildProcess::disconnect()
{
    Isolate* isolate = holder();

    if (!m_channel)
        return CHECK_ERROR(Runtime::setError("ChildProcess: IPC channel is already disconnected."));

    obj_ptr<Stream_base> _channel = m_channel;
    m_channel.Release();

    _emit("disconnect");

    _channel->ac_close();

    return 0;
}

result_t ChildProcess::send(v8::Local<v8::Value> msg)
{
    if (m_ipc < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return Ipc::send(m_stdio[3], msg);
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