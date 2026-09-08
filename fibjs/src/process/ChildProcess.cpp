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
#include "AbortController.h"
#include <signal.h>
#include <mutex>

#ifndef _WIN32
#include <sys/ioctl.h>
#include <termios.h>
#endif

namespace fibjs {

// Timer that kills a child process after a timeout
struct KillTimer {
    uv_timer_t timer;
    obj_ptr<ChildProcess> cp;
    exlib::string killSignal;

    KillTimer(ChildProcess* _cp, const exlib::string& _killSignal)
        : cp(_cp)
        , killSignal(_killSignal)
    {
        timer.data = this;
    }

    static void on_timer(uv_timer_t* handle)
    {
        KillTimer* self = (KillTimer*)handle->data;
        self->cp->kill(self->killSignal);
        // Close stdout/stderr so copyTo() unblocks even if grandchildren still
        // hold the write end of the pipe (e.g. "sh -c sleep 10" on Linux).
        if (self->cp->m_stdio[1])
            self->cp->m_stdio[1]->close(nullptr);
        if (self->cp->m_stdio[2])
            self->cp->m_stdio[2]->close(nullptr);
    }
};

// Children that are still running: pids are registered right after a
// successful spawn (uv loop thread) and removed in OnExit (PTY children
// funnel through the same uv_process_t exit_cb). killAliveChildren() only
// snapshots under the lock and then SIGKILLs, so it can run on any thread
// (the test watchdog does not depend on the uv loop being responsive).
static std::vector<int32_t> s_aliveChildren;
static std::mutex s_aliveLock;

void ChildProcess::registerChild(int32_t pid)
{
    std::lock_guard<std::mutex> l(s_aliveLock);
    s_aliveChildren.push_back(pid);
}

void ChildProcess::unregisterChild(int32_t pid)
{
    std::lock_guard<std::mutex> l(s_aliveLock);
    for (auto it = s_aliveChildren.begin(); it != s_aliveChildren.end(); it++)
        if (*it == pid) {
            s_aliveChildren.erase(it);
            break;
        }
}

int32_t ChildProcess::killAliveChildren()
{
    std::vector<int32_t> pids;

    {
        std::lock_guard<std::mutex> l(s_aliveLock);
        pids = s_aliveChildren;
    }

    int32_t n = 0;

    for (int32_t pid : pids)
#ifdef SIGKILL
        if (uv_kill(pid, SIGKILL) == 0)
#else
        if (uv_kill(pid, 9) == 0)
#endif
            n++;

    return n;
}

// Bridge for the test watchdog (src/test/test.cpp), which cannot include
// ChildProcess.h because that header #undef's stdout/stderr.
int32_t child_process_kill_alive_children()
{
    return ChildProcess::killAliveChildren();
}

void ChildProcess::on_uv_close(uv_handle_t* handle)
{
    ChildProcess* cp = container_of(handle, ChildProcess, m_process);

    // If no stdout pipe exists, trigger close event on process close
    if (!cp->m_stdio[1]) {
        cp->m_stdoutClosed = true;
        cp->try_emit_close();
    }

    cp->isolate_unref();
}

void ChildProcess::try_emit_close()
{
    // Only emit close when both process has exited AND stdout is closed
    if (m_exited && m_stdoutClosed)
        emit_close();
}

void ChildProcess::emit_close()
{
    Variant args[2];

    if (m_exitCode < 0) {
        // Killed by a signal: report null code and the signal name,
        // matching Node's close event semantics.
        args[0].setNull();
        args[1] = signo_string(-m_exitCode);
    } else {
        args[0] = m_exitCode;
        args[1].setNull();
    }

    // Clean up PTY resources on Windows
#ifdef _WIN32
    if (m_pty) {
        pty_cleanup(&m_process);
    }
#endif

    _emit("close", args, 2);
    m_vholder.Release();
}

void ChildProcess::OnExit(uv_process_t* handle, int64_t exit_status, int term_signal)
{
    ChildProcess* cp = container_of(handle, ChildProcess, m_process);

    unregisterChild(handle->pid);

    // Stop the kill timer if active (both OnExit and timer run in uv loop)
    if (cp->m_killTimer) {
        uv_timer_stop(&cp->m_killTimer->timer);
        uv_close((uv_handle_t*)&cp->m_killTimer->timer, [](uv_handle_t* h) {
            delete (KillTimer*)h->data;
        });
        cp->m_killTimer = nullptr;
    }

    Variant args[2];

    if (term_signal) {
        exit_status = -term_signal;
        // Killed by a signal: report null code and the signal name,
        // matching Node's exit event semantics. The negative signal code is
        // kept in m_exitCode for join()/spawnSync() internal use.
        args[0].setNull();
        args[1] = signo_string(term_signal);
    } else {
        args[0] = (double)exit_status;
        args[1].setNull();
    }

    cp->m_exitCode = (int32_t)exit_status;
    cp->m_ev.set();

    cp->_emit("exit", args, 2);
    cp->m_exited = true;
    cp->try_emit_close();

    uv_close((uv_handle_t*)handle, on_uv_close);
}

result_t ChildProcess::create_pipe(int32_t idx)
{
    // Only stdout (idx=1) triggers close event when closed
    std::function<void(int32_t)> onClose = nullptr;
    if (idx == 1) {
        // prevent GC from releasing this before the callback is called
        Ref();
        onClose = [this](int32_t fd) -> void {
            m_stdoutClosed = true;
            try_emit_close();
            Unref();
        };
    }

    result_t hr = UVStream::create_pipe(m_stdio[idx], m_ipc == idx, onClose);
    if (hr < 0)
        return hr;

    stdios[idx].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
    stdios[idx].data.stream = (uv_stream_t*)&m_stdio[idx]->m_pipe;

    if (idx + 1 > (int32_t)uv_options.stdio_count)
        uv_options.stdio_count = idx + 1;

    return 0;
}

result_t ChildProcess::fill_stdio(v8::Local<v8::Object> options, bool fork)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    int32_t i;

    // libuv allows up to 3 extra pipe fds beyond the standard three (0-2),
    // so stdio array entries 3-5 are supported (chromium/playwright use 5).
    enum { MAX_STDIO = 6 };

    Variant stddefs[MAX_STDIO];
    int32_t count = 3;
    v8::Local<v8::Value> v;
    hr = GetConfigValue(options, "stdio", v);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        // fork 默认 stdio 为 inherit（与 Node 一致）；silent:true 时改为 pipe。
        // Node 语义：silent 仅在未显式提供 stdio 时生效，spawn 没有 silent 选项。
        bool silent = false;
        GetConfigValue(options, "silent", silent);

        for (i = 0; i < 3; i++)
            stddefs[i] = fork ? (silent ? "pipe" : "inherit") : "pipe";
    } else {
        exlib::string s;
        hr = GetArgumentValue(isolate, v, s, true);
        if (hr >= 0) {
            for (i = 0; i < 3; i++)
                stddefs[i] = s;
        } else {
            v8::Local<v8::Array> a;
            hr = GetArgumentValue(isolate, v, a, true);
            if (hr >= 0) {
                count = a->Length();
                if (count > MAX_STDIO)
                    count = MAX_STDIO;
                for (i = 0; i < count; i++)
                    stddefs[i] = JSValue(a->Get(context, i));
            } else {
                for (i = 0; i < 3; i++)
                    stddefs[i] = "pipe";
            }
        }
    }

    uv_options.stdio = stdios;
    uv_options.stdio_count = 0;

    int32_t pty_cnt = 0;
    for (i = 0; i < count; i++)
        if (stddefs[i].type() == Variant::VT_String && stddefs[i].string() == "pty")
            pty_cnt++;

    if (pty_cnt == count) {
        m_pty = true;

        for (i = 0; i < count; i++) {
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

    for (i = 0; i < count; i++) {
        if (stddefs[i].type() == Variant::VT_Integer
            || stddefs[i].type() == Variant::VT_Long
            || stddefs[i].type() == Variant::VT_Number) {
            double fd_num = stddefs[i].dblVal();
            int32_t fd = stddefs[i].intVal();

            if (fd_num < 0 || fd_num != (double)fd)
                return CHECK_ERROR(Runtime::setError("ChildProcess: stdio fd must be a non-negative integer."));

            stdios[i].flags = UV_INHERIT_FD;
            stdios[i].data.fd = fd;
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

    if (uv_options.stdio_count < count)
        uv_options.stdio_count = count;

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
    hr = GetConfigValue(options, "uid", uid);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;

        uv_options.flags |= UV_PROCESS_SETUID;
        uv_options.uid = (uv_uid_t)uid;
    }

    int32_t gid;
    hr = GetConfigValue(options, "gid", gid);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;

        uv_options.flags |= UV_PROCESS_SETGID;
        uv_options.gid = (uv_gid_t)gid;
    }

    v8::Local<v8::Object> opt_envs;
    v8::Local<v8::Value> opt_envs_v;
    GetConfigValue(options, "env", opt_envs_v);
    if (IsEmpty(opt_envs_v)) {
        hr = process_base::get_env(opt_envs);
        if (hr < 0)
            return hr;
    } else if (opt_envs_v->IsObject())
        opt_envs = opt_envs_v->ToObject(context).ToLocalChecked();
    else
        return CALL_E_TYPEMISMATCH;

    JSArray keys = opt_envs->GetPropertyNames(opt_envs->GetCreationContextChecked());
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

        hr = GetArgumentValue(isolate, k, ks);
        if (hr < 0)
            return hr;

        if (ks == "NODE_CHANNEL_FD")
            continue;

        if (IsEmpty(v))
            continue;

        hr = GetArgumentValue(isolate, v, vs);
        if (hr < 0)
            return hr;

        ks.append(1, '=');
        ks.append(vs);

        _envs[p] = (char*)envStr[p].c_str();
        p++;
    }

    if (m_ipc >= 0) {
        exlib::string& ks = envStr[p];
        exlib::string v;

        ks = "NODE_CHANNEL_FD";
        ks.append(1, '=');
        ks.append(1, '0' + m_ipc);

        _envs[p] = (char*)envStr[p].c_str();
        p++;
    }

    _envs[p] = NULL;
    uv_options.env = _envs.data();

    return 0;
}

result_t ChildProcess::fill_arg(v8::Local<v8::Array> args)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    int32_t len, i;

    len = args.IsEmpty() ? 0 : args->Length();
    argStr.resize(len);
    _args.resize(len + 2);

    _args[0] = (char*)m_command.c_str();
    for (i = 0; i < len; i++) {
        hr = GetArgumentValue(isolate, JSValue(args->Get(context, i)), argStr[i]);
        if (hr < 0)
            return hr;

        _args[i + 1] = (char*)argStr[i].c_str();
    }
    _args[i + 1] = NULL;

    uv_options.args = _args.data();
    uv_options.file = m_command.c_str();

    return 0;
}

result_t ChildProcess::fill_opt(v8::Local<v8::Object> options)
{
    process_base::cwd(cwd);
    GetConfigValue(options, "cwd", cwd);
    uv_options.cwd = cwd.c_str();

    bool detached = false;
    GetConfigValue(options, "detached", detached);
    if (detached)
        uv_options.flags |= UV_PROCESS_DETACHED;

    bool windowsVerbatimArguments = false;
    GetConfigValue(options, "windowsVerbatimArguments", windowsVerbatimArguments);
    if (windowsVerbatimArguments)
        uv_options.flags |= UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS;

    bool windowsHide = false;
    GetConfigValue(options, "windowsHide", windowsHide);
    if (windowsHide)
        uv_options.flags |= UV_PROCESS_WINDOWS_HIDE;

    // Parse PTY options
    GetConfigValue(options, "cols", m_cols);
    GetConfigValue(options, "rows", m_rows);

    return 0;
}
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

    m_command = command;
    hr = fill_arg(args);
    if (hr < 0)
        return hr;

    hr = fill_opt(options);
    if (hr < 0)
        return hr;

    int32_t timeout = 0;
    GetConfigValue(options, "timeout", timeout);

    exlib::string killSignal("SIGTERM");
    GetConfigValue(options, "killSignal", killSignal);

    isolate_ref();
    m_vholder = new ValueHolder(wrap());

    hr = uv_call([&] {
        int32_t err;

        if (m_pty) {
            int32_t stdinfd, stdoutfd;
            err = pty_spawn(s_uv_loop, &m_process, &uv_options, &stdinfd, &stdoutfd, m_cols, m_rows);
            if (err >= 0) {
                m_stdinfd = stdinfd;
                m_stdoutfd = stdoutfd;

                // Create separate streams for stdin (write-only) and stdout (read-only)
                // stdout (idx=1) triggers close event when closed
                UVStream::uv_pipe(m_stdio[0], stdinfd, nullptr);

                UVStream::uv_pipe(m_stdio[1], stdoutfd, [this](int32_t fd) -> void {
                    m_stdoutClosed = true;
                    try_emit_close();
                });
            }
        } else
            err = uv_spawn(s_uv_loop, &m_process, &uv_options);

        if (err < 0)
            uv_close((uv_handle_t*)&m_process, on_uv_close);
        else {
            registerChild(m_process.pid);

            _emit("spawn");

            // Start kill timer in the same uv loop iteration as spawn,
            // so OnExit cannot fire before the timer is initialized.
            if (timeout > 0) {
                m_killTimer = new KillTimer(this, killSignal);
                uv_timer_init(s_uv_loop, &m_killTimer->timer);
                uv_timer_start(&m_killTimer->timer, KillTimer::on_timer, timeout, 0);
            }
        }

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

    obj_ptr<AbortSignal_base> abortSignal;
    GetConfigValue(options, "signal", abortSignal);
    if (abortSignal) {
        AbortSignal* signal = abortSignal.As<AbortSignal>();
        if (signal->is_aborted()) {
            kill("SIGTERM");
        } else {
            this->Ref();
            signal->addAbortCallback([this]() {
                kill("SIGTERM");
                Unref();
            });
        }
    }

    return hr;
}

result_t ChildProcess::kill(int32_t signal)
{
    m_killed = true;
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
        _ac->apost(0);
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
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "send: IPC channel is not available."));

    return Ipc::send(m_stdio[3], msg);
}

result_t ChildProcess::get_pid(int32_t& retVal)
{
    retVal = m_process.pid;
    return 0;
}

result_t ChildProcess::get_killed(bool& retVal)
{
    retVal = m_killed;
    return 0;
}

result_t ChildProcess::get_exitCode(int32_t& retVal)
{
    if (!m_ev.isSet())
        return CALL_RETURN_NULL;

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

result_t ChildProcess::get_stdio(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    retVal = v8::Array::New(isolate->m_isolate, 6);
    for (int32_t i = 0; i < 6; i++) {
        if (m_stdio[i])
            retVal->Set(context, i, m_stdio[i]->wrap());
        else
            retVal->Set(context, i, v8::Null(isolate->m_isolate));
    }

    return 0;
}

result_t ChildProcess::resize(int32_t cols, int32_t rows)
{
    if (!m_pty)
        return CHECK_ERROR(Runtime::setError("resize() only available in PTY mode"));

    if (cols <= 0 || rows <= 0)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "resize: cols and rows must be positive, received %d x %d.", cols, rows));

    if (m_stdinfd == -1 && m_stdoutfd == -1)
        return CHECK_ERROR(Runtime::setError("PTY not available"));

    // Update stored cols and rows
    m_cols = cols;
    m_rows = rows;

#ifdef _WIN32
    // Call resize function with process handle on Windows
    int result = pty_resize(&m_process, cols, rows);
    return result == 0 ? 0 : CHECK_ERROR(Runtime::setError("resize failed"));
#else
    // On POSIX systems, call ioctl directly
    struct winsize winsize;
    winsize.ws_col = cols;
    winsize.ws_row = rows;
    winsize.ws_xpixel = 0;
    winsize.ws_ypixel = 0;

    // Use whichever fd is available (both should point to the same pty)
    int fd_to_use = (m_stdinfd != -1) ? m_stdinfd : m_stdoutfd;
    int result = ioctl(fd_to_use, TIOCSWINSZ, &winsize);
    return result == 0 ? 0 : CHECK_ERROR(Runtime::setError("resize failed"));
#endif
}

result_t ChildProcess::get_cols(int32_t& retVal)
{
    if (!m_pty)
        return CHECK_ERROR(Runtime::setError("cols property only available in PTY mode"));
    retVal = m_cols;
    return 0;
}

result_t ChildProcess::get_rows(int32_t& retVal)
{
    if (!m_pty)
        return CHECK_ERROR(Runtime::setError("rows property only available in PTY mode"));
    retVal = m_rows;
    return 0;
}

result_t ChildProcess::ref(obj_ptr<ChildProcess_base>& retVal)
{
    object_base::isolate_ref();
    retVal = this;
    return 0;
}

result_t ChildProcess::unref(obj_ptr<ChildProcess_base>& retVal)
{
    object_base::isolate_unref();
    retVal = this;
    return 0;
}

result_t ChildProcess::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    // When "close" event is listened, start reading stdout to ensure close event is triggered
    if (ev == "close" && m_stdio[1])
        m_stdio[1]->startRecvStream();

    return 0;
}

}