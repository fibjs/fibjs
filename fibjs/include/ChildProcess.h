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

namespace fibjs {

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
        : m_exitCode(-1)
        , m_ipc(-1)
        , m_pty(false)
    {
        memset(&uv_options, 0, sizeof(uv_process_options_t));
        uv_options.exit_cb = OnExit;
    }

    FIBER_FREE();
    EVENT_SUPPORT();

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
    virtual result_t get_exitCode(int32_t& retVal);
    virtual result_t get_stdin(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stdout(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stderr(obj_ptr<Stream_base>& retVal);

public:
    static int32_t spawn(uv_process_t* process, const uv_process_options_t* options);
    result_t spawn(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, bool fork);

public:
    EVENT_FUNC(exit);
    EVENT_FUNC(message);

private:
    result_t create_pipe(int32_t idx);
    result_t fill_stdio(v8::Local<v8::Object> options, bool fork);
    result_t fill_env(v8::Local<v8::Object> options);
    result_t fill_arg(exlib::string command, v8::Local<v8::Array> args);
    result_t fill_opt(v8::Local<v8::Object> options);

public:
    static void on_uv_close(uv_handle_t* handle);
    static void OnExit(uv_process_t* handle, int64_t exit_status, int term_signal);

public:
    exlib::Event m_ev;
    obj_ptr<ValueHolder> m_vholder;

    obj_ptr<UVStream> m_stdio[4];

    uv_stdio_container_t stdios[4];
    uv_process_options_t uv_options;
    uv_process_t m_process;

    obj_ptr<Stream_base> m_channel;

    int32_t m_ipc;

    bool m_pty;

    int32_t m_exitCode;

    std::vector<char*> envp;

    std::vector<exlib::string> argStr;
    std::vector<char*> _args;

    std::vector<exlib::string> envStr;
    std::vector<char*> _envs;

    exlib::string cwd;
};
}
