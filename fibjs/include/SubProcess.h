/*
 * SubProcess.h
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_

#include "ifs/SubProcess.h"
#include "ifs/io.h"
#include "AsyncIO.h"
#include "Timer.h"

namespace fibjs {

class SubProcess : public SubProcess_base {
public:
    SubProcess(intptr_t pid, intptr_t ppid)
        : m_pid(pid)
        , m_ppid(ppid)
    {
    }

#ifdef _WIN32
    ~SubProcess()
    {
        ::CloseHandle((HANDLE)m_pid);
    }
#endif

    FIBER_FREE();

public:
    // SubProcess_base
    virtual result_t kill(int32_t signal);
    virtual result_t wait(int32_t& retVal, AsyncEvent* ac);
    virtual result_t findWindow(exlib::string name, v8::Local<v8::Value>& retVal);
    virtual result_t get_pid(int32_t& retVal);
    virtual result_t get_stdin(obj_ptr<BufferedStream_base>& retVal);
    virtual result_t get_stdout(obj_ptr<BufferedStream_base>& retVal);
    virtual result_t get_stderr(obj_ptr<BufferedStream_base>& retVal);

public:
    static result_t create(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts,
        bool redirect, obj_ptr<SubProcess_base>& retVal);

public:
    class Pipe : public Stream_base {
        FIBER_FREE();

    public:
        Pipe(intptr_t fd)
            : m_aio(fd, 0, -1)
        {
        }

        ~Pipe()
        {
            if (m_aio.m_fd != INVALID_SOCKET)
#ifdef _WIN32
                asyncCall(::CloseHandle, (HANDLE)m_aio.m_fd);
#else
                asyncCall(::close, m_aio.m_fd);
#endif
        }

    public:
        // Stream_base
        virtual result_t get_fd(int32_t& retVal)
        {
            retVal = *(int32_t*)&m_aio.m_fd;
            return 0;
        }

        virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
            AsyncEvent* ac)
        {
            return m_aio.read(bytes, retVal, ac, bytes > 0, NULL);
        }

        virtual result_t write(Buffer_base* data, AsyncEvent* ac)
        {
            return m_aio.write(data, ac);
        }

        virtual result_t flush(AsyncEvent* ac)
        {
            return 0;
        }

        virtual result_t close(AsyncEvent* ac)
        {
            if (m_aio.m_fd == INVALID_SOCKET)
                return 0;

            if (ac->isSync())
                return CHECK_ERROR(CALL_E_NOSYNC);

            return m_aio.close(ac);
        }

        virtual result_t copyTo(Stream_base* stm, int64_t bytes,
            int64_t& retVal, AsyncEvent* ac)
        {
            if (m_aio.m_fd == INVALID_SOCKET)
                return CHECK_ERROR(CALL_E_INVALID_CALL);

            return io_base::copyStream(this, stm, bytes, retVal, ac);
        }

    private:
        AsyncIO m_aio;
    };

    static void wrap_pipe(intptr_t fd, obj_ptr<BufferedStream_base>& bs);

public:
    exlib::Event m_exit;
    int32_t m_status;

private:
    obj_ptr<Timer> m_timer;
    obj_ptr<BufferedStream_base> m_stdin;
    obj_ptr<BufferedStream_base> m_stdout;
    obj_ptr<BufferedStream_base> m_stderr;

    intptr_t m_pid;
    intptr_t m_ppid;
};

} /* namespace fibjs */

#endif /* SUBPROCESS_H_ */
