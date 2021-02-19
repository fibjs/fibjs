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
            if (is_stdio_fd(fd) && uv_guess_handle(fd) == UV_TTY) {
                int ret = uv_tty_init(s_uv_loop, &m_tty, fd, 0);
                if (ret != 0)
                    return ret;

                return uv_stream_set_blocking(&m_stream, 1);
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
    class AsyncRead : public exlib::linkitem {
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
        result_t start()
        {
            return uv_call([&] {
                m_this->queue_read.putTail(this);
                if (m_this->queue_read.count() == 1) {
                    int ret = uv_read_start(&m_this->m_stream, on_alloc, on_read);
                    if (ret) {
                        m_this->queue_read.getHead();
                        delete this;
                        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));
                    }
                }

                return CALL_E_PENDDING;
            });
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
                uv_read_stop(&pThis->m_stream);
                post_all_data(pThis);
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

        static void post_all_data(UVStream* pThis)
        {
            while (pThis->queue_read.count())
                pThis->queue_read.getHead()->post_data();
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

    class AsyncWrite : public exlib::linkitem {
    public:
        AsyncWrite(UVStream* pThis, Buffer_base* data, AsyncEvent* ac)
            : m_this(pThis)
            , m_ac(ac)
        {
            data->toString(m_strBuf);
            m_buf.base = (char*)m_strBuf.c_str();
            m_buf.len = (uint32_t)m_strBuf.length();
        }

    public:
        result_t start()
        {
            return uv_call([&] {
                m_this->queue_write.putTail(this);
                if (m_this->queue_write.count() == 1) {
                    int ret = uv_write(&m_req, &m_this->m_stream, &m_buf, 1, on_write);
                    if (ret) {
                        m_this->queue_write.getHead();
                        delete this;
                        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));
                    }
                }

                return CALL_E_PENDDING;
            });
        }

    public:
        static void on_write(uv_write_t* req, int status)
        {
            UVStream* pThis = container_of(req->handle, UVStream, m_handle);
            AsyncWrite* wr;

            if (status < 0) {
                post_all_result(pThis, status);
                return;
            }

            pThis->queue_write.getHead()->post_result(0);

            if (pThis->queue_write.count() > 0) {
                wr = pThis->queue_write.head();
                int ret = uv_write(&wr->m_req, &pThis->m_stream, &wr->m_buf, 1, on_write);
                if (ret)
                    post_all_result(pThis, ret);
            }
        }

        static void post_all_result(UVStream* pThis, int status)
        {
            if (!pThis->queue_write.count())
                return;

            result_t hr = Runtime::setError(uv_strerror(status));
            while (pThis->queue_write.count())
                pThis->queue_write.getHead()->post_result(hr);
        }

        void post_result(result_t hr)
        {
            m_ac->apost(hr);
            delete this;
        }

    private:
        obj_ptr<UVStream> m_this;
        AsyncEvent* m_ac;
        exlib::string m_strBuf;
        uv_buf_t m_buf;
        uv_write_t m_req;
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

        return (new AsyncRead(this, bytes, retVal, ac))->start();
    }

    virtual result_t write(Buffer_base* data, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new AsyncWrite(this, data, ac))->start();
    }

    virtual result_t flush(AsyncEvent* ac)
    {
        return 0;
    }

    static void on_close(uv_handle_t* handle)
    {
        UVStream* pThis = container_of(handle, UVStream, m_handle);

        AsyncRead::post_all_data(pThis);
        AsyncWrite::post_all_result(pThis, EPIPE);

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
    exlib::List<AsyncWrite> queue_write;
    AsyncEvent* ac_close;
};
}

#endif // _UVSTREAM_H