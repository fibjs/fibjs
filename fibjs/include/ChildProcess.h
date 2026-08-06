/*
 * ChildProcess.h
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#pragma once

#include "ifs/ChildProcess.h"
#include "AsyncUV.h"
#include "UVStream.h"

#undef stdout
#undef stderr
#include "ifs/child_process.h"

// PTY function declarations
extern "C" {
int pty_spawn(uv_loop_t* loop, uv_process_t* process, const uv_process_options_t* options, int* stdinfd, int* stdoutfd, int cols, int rows);
int pty_resize(uv_process_t* process, int cols, int rows);
void pty_cleanup(uv_process_t* process);
}

namespace fibjs {

struct KillTimer;

class ChildProcess : public ChildProcess_base {
public:
    class Ipc {
    public:
        Ipc(Isolate* _isolate, v8::Local<v8::Object> _o, obj_ptr<Stream_base>& stream);

        static result_t send(Stream_base* stream, v8::Local<v8::Value> msg);
        static result_t sync_delete(Ipc* pThis);

    public:
        Isolate* m_isolate;
        v8::Global<v8::Object> m_o;
        obj_ptr<Stream_base> m_stream;
        obj_ptr<Stream_base>& m_channel;
    };

public:
    ChildProcess()
        : m_ipc(-1)
        , m_pty(false)
        , m_killed(false)
        , m_cols(80)
        , m_rows(24)
        , m_stdinfd(-1)
        , m_stdoutfd(-1)
        , m_exited(false)
        , m_stdoutClosed(false)
        , m_killTimer(nullptr)
    {
        memset(&uv_options, 0, sizeof(uv_process_options_t));
        uv_options.exit_cb = OnExit;
    }

    FIBER_FREE();

public:
    // ChildProcess_base
    virtual result_t kill(int32_t signal);
    virtual result_t kill(exlib::string signal);
    virtual result_t join(int32_t& retVal, AsyncEvent* ac);
    virtual result_t get_connected(bool& retVal);
    virtual result_t disconnect();
    virtual result_t send(v8::Local<v8::Value> msg);
    virtual result_t usage(v8::Local<v8::Object>& retVal);
    virtual result_t get_pid(int32_t& retVal);
    virtual result_t get_killed(bool& retVal);
    virtual result_t get_exitCode(int32_t& retVal);
    virtual result_t get_stdin(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stdout(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stderr(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stdio(v8::Local<v8::Array>& retVal);
    virtual result_t resize(int32_t cols, int32_t rows);
    virtual result_t get_cols(int32_t& retVal);
    virtual result_t get_rows(int32_t& retVal);
    virtual result_t ref(obj_ptr<ChildProcess_base>& retVal);
    virtual result_t unref(obj_ptr<ChildProcess_base>& retVal);

public:
    // object_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func);

public:
    static int32_t spawn(uv_process_t* process, const uv_process_options_t* options);
    result_t spawn(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, bool fork);

public:
    static result_t async_spawn(exlib::string command, v8::Local<v8::Array> args,
        v8::Local<v8::Object> options, obj_ptr<child_process_base::SpawnSyncType>& retVal, AsyncEvent* ac);
    ASYNC_STATICVALUE4(ChildProcess, async_spawn, exlib::string, v8::Local<v8::Array>, v8::Local<v8::Object>, obj_ptr<child_process_base::SpawnSyncType>);

private:
    result_t create_pipe(int32_t idx);
    result_t fill_stdio(v8::Local<v8::Object> options, bool fork);
    result_t fill_env(v8::Local<v8::Object> options);
    result_t fill_arg(v8::Local<v8::Array> args);
    result_t fill_opt(v8::Local<v8::Object> options);

public:
    static void on_uv_close(uv_handle_t* handle);
    static void OnExit(uv_process_t* handle, int64_t exit_status, int term_signal);
    void emit_close();
    void try_emit_close();

public:
    exlib::Event m_ev;
    obj_ptr<ValueHolder> m_vholder;

    // stdio slots 0-5: 0=stdin, 1=stdout, 2=stderr, 3-5=extra pipes
    // (libuv allows up to 3 extra pipe fds beyond the standard three)
    obj_ptr<UVStream> m_stdio[6];

    uv_stdio_container_t stdios[6];
    uv_process_options_t uv_options;
    uv_process_t m_process;

    obj_ptr<Stream_base> m_channel;

    int32_t m_ipc;

    bool m_pty;
    int32_t m_cols;
    int32_t m_rows;
    int32_t m_stdinfd;
    int32_t m_stdoutfd;

    bool m_killed;
    int32_t m_exitCode;
    bool m_exited;
    bool m_stdoutClosed;

    KillTimer* m_killTimer;

    std::vector<char*> envp;

    exlib::string m_command;

    std::vector<exlib::string> argStr;
    std::vector<char*> _args;

    std::vector<exlib::string> envStr;
    std::vector<char*> _envs;

    exlib::string cwd;
};
}
