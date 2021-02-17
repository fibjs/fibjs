/*
 * ChildProcess.h
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#include "ifs/ChildProcess.h"
#include "AsyncUV.h"

#ifndef _CHILDPROCESS_H
#define _CHILDPROCESS_H

namespace fibjs {

class ChildProcess : public ChildProcess_base {
public:
    ChildProcess()
        : m_exitCode(-1)
    {
        memset(&uv_options, 0, sizeof(uv_process_options_t));
        uv_options.exit_cb = OnExit;
    }

    FIBER_FREE();
    EVENT_SUPPORT();

public:
    // ChildProcess_base
    virtual result_t kill(int32_t signal);
    virtual result_t join(AsyncEvent* ac);
    virtual result_t get_pid(int32_t& retVal);
    virtual result_t get_exitCode(int32_t& retVal);
    virtual result_t get_stdin(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stdout(obj_ptr<Stream_base>& retVal);
    virtual result_t get_stderr(obj_ptr<Stream_base>& retVal);

public:
    static int32_t spawn(uv_process_t* process, const uv_process_options_t* options);

public:
    EVENT_FUNC(exit);

public:
    result_t fill_stdio(v8::Local<v8::Object> options);
    result_t fill_env(v8::Local<v8::Object> options);
    result_t fill_arg(exlib::string command, v8::Local<v8::Array> args);
    result_t fill_opt(v8::Local<v8::Object> options);
    result_t spawn(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options);

public:
    static void on_uv_close(uv_handle_t* handle);
    static void OnExit(uv_process_t* handle, int64_t exit_status, int term_signal);

public:
    exlib::Event m_ev;
    obj_ptr<ValueHolder> m_vholder;

    obj_ptr<Stream_base> m_stdio[3];

    uv_stdio_container_t stdios[3];
    uv_process_options_t uv_options;
    uv_process_t m_process;

    int32_t m_exitCode;

    std::vector<char*> envp;

    std::vector<exlib::string> argStr;
    std::vector<char*> _args;

    std::vector<exlib::string> envStr;
    std::vector<char*> _envs;

    exlib::string cwd;
};
}

#endif // _CHILDPROCESS_H
