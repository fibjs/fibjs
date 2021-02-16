/*
 * UVStream.h
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#include "ifs/io.h"
#include "AsyncUV.h"
#include "Buffer.h"

#ifndef _UVSTREAM_H
#define _UVSTREAM_H

namespace fibjs {

inline bool is_stdio_fd(int32_t fd)
{
    return fd >= 0 && fd <= 2;
}

class UVStream : public Stream_base {
    FIBER_FREE();

public:
    UVStream(int32_t fd)
    {
        m_fd = fd;

        uv_call([&] {
            if (is_stdio_fd(fd)) {
                uv_handle_type type = uv_guess_handle(fd);
                if (type == UV_TTY)
                    return uv_tty_init(s_uv_loop, &m_tty, fd, fd == 0);
            }

            uv_pipe_init(s_uv_loop, &m_pipe, 0);
            return uv_pipe_open(&m_pipe, fd);
        });
    }

    UVStream()
    {
        m_fd = -1;

        uv_call([&] {
            return uv_pipe_init(s_uv_loop, &m_pipe, 0);
        });
    }

    static void on_delete(uv_handle_t* handle)
    {
        UVStream* pThis = container_of(handle, UVStream, m_handle);
        delete pThis;
    }

    virtual void Delete()
    {
        result_t hr = uv_call([&] {
            if (uv_is_closing(&m_handle))
                return CALL_E_INVALID_CALL;

            uv_close(&m_handle, on_delete);
            return CALL_E_PENDDING;
        });

        if (hr != CALL_E_PENDDING)
            delete this;
    }

public:
    // Stream_base
    class AsyncRead : public AsyncEvent {
    public:
        AsyncRead(UVStream* pThis, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
            : m_this(pThis)
            , m_bytes(bytes)
            , m_retVal(retVal)
            , m_ac(ac)
            , m_pos(0)
        {
        }

    public:
        int start()
        {
            m_this->queue_read.putTail(this);
            if (m_this->queue_read.count() == 1)
                return uv_read_start(&m_this->m_stream, on_alloc, on_read);

            return 0;
        }

    public:
        static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
        {
            AsyncRead* ar = container_of(handle, UVStream, m_handle)->queue_read.head();

            if (ar->m_buf.empty()) {
                if (ar->m_bytes > 0)
                    suggested_size = ar->m_bytes;
                ar->m_buf.resize(suggested_size);
            }

            buf->base = ar->m_buf.c_buffer() + ar->m_pos;
            buf->len = (uint32_t)ar->m_buf.length() - ar->m_pos;
        }

        static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
        {
            UVStream* pThis = container_of(stream, UVStream, m_handle);
            AsyncRead* ar;

            if (nread < 0) {
                while (pThis->queue_read.count()) {
                    ar = pThis->queue_read.getHead();
                    ar->post_data();
                }
                return;
            }

            ar = pThis->queue_read.head();
            ar->m_pos += (int32_t)nread;

            if ((ar->m_bytes < 0) || (ar->m_bytes == ar->m_pos)) {
                pThis->queue_read.getHead();
                ar->post_data();

                if (pThis->queue_read.count() == 0)
                    uv_read_stop(&pThis->m_stream);
            }
        }

        void post_data()
        {
            if (m_pos) {
                if (m_pos < m_buf.length())
                    m_buf.resize(m_pos);

                m_retVal = new Buffer(m_buf);
                m_ac->apost(0);
            } else
                m_ac->apost(CALL_RETURN_NULL);

            delete this;
        }

    private:
        obj_ptr<UVStream> m_this;
        int32_t m_bytes;
        obj_ptr<Buffer_base>& m_retVal;
        AsyncEvent* m_ac;
        int32_t m_pos;
        exlib::string m_buf;
    };

    virtual result_t get_fd(int32_t& retVal)
    {
        if (is_stdio_fd(m_fd)) {
            retVal = m_fd;
            return 0;
        }

        uv_os_fd_t fileno;

        int ret = uv_fileno(&m_handle, &fileno);
        if (ret != 0)
            return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));
        retVal = *(int32_t*)&fileno;

        return 0;
    };

    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        int ret = uv_call([&] {
            return (new AsyncRead(this, bytes, retVal, ac))->start();
        });

        if (ret != 0)
            return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));
        return CALL_E_PENDDING;
    }

    virtual result_t write(Buffer_base* data, AsyncEvent* ac)
    {
        class write_req : public uv_write_t {
        public:
            write_req(Buffer_base* data, AsyncEvent* ac)
                : m_ac(ac)
            {
                data->toString(strBuf);
                buf.base = (char*)strBuf.c_str();
                buf.len = (uint32_t)strBuf.length();
            }

        public:
            static void on_write(uv_write_t* req, int status)
            {
                write_req* r = (write_req*)req;
                r->m_ac->apost(0);
                delete r;
            }

        public:
            exlib::string strBuf;
            AsyncEvent* m_ac;
            uv_buf_t buf;
        };

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        write_req* req = new write_req(data, ac);
        int ret = uv_call([&] {
            return uv_write(req, &m_stream, &req->buf, 1, write_req::on_write);
        });

        if (ret != 0)
            return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));
        return CALL_E_PENDDING;
    }

    virtual result_t flush(AsyncEvent* ac)
    {
        return 0;
    }

    static void on_close(uv_handle_t* handle)
    {
        UVStream* pThis = container_of(handle, UVStream, m_handle);

        if (pThis->queue_read.count())
            AsyncRead::on_read(&pThis->m_stream, -1, NULL);
        pThis->ac_close->apost(0);
    }

    virtual result_t close(AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return uv_call([&] {
            if (uv_is_closing(&m_handle))
                return CALL_E_INVALID_CALL;

            ac_close = ac;
            uv_close(&m_handle, on_close);
            return CALL_E_PENDDING;
        });
    }

    virtual result_t copyTo(Stream_base* stm, int64_t bytes,
        int64_t& retVal, AsyncEvent* ac)
    {
        return io_base::copyStream(this, stm, bytes, retVal, ac);
    }

private:
    int32_t m_fd;

public:
    union {
        uv_handle_t m_handle;
        uv_stream_t m_stream;
        uv_pipe_t m_pipe;
        uv_tty_t m_tty;
    };
    exlib::List<AsyncRead> queue_read;
    AsyncEvent* ac_close;
};
}

#endif // _UVSTREAM_H