/*
 * UVStream.h
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#pragma once

#include "ifs/io.h"
#include "AsyncUV.h"
#include "Buffer.h"
#include "AsyncStream.h"
#include "options.h"
#include "ifs/console.h"

#define STREAM_BLOCK_SIZE 2048

namespace fibjs {

template <typename T>
class UVStream_tmpl : public AsyncStream<T> {
public:
    class UVTimeout : public uv_timer_t {
    public:
        UVTimeout(UVStream_tmpl* _this)
            : m_this(_this)
            , m_timeout(_this->m_timeout)
            , m_timer_started(false)
        {
        }

        UVTimeout(UVStream_tmpl* _this, int32_t timeout)
            : m_this(_this)
            , m_timeout(timeout)
            , m_timer_started(false)
        {
        }

        // Must be called in uv loop thread (from invoke())
        void start_timer()
        {
            if (m_timeout > 0 && !m_timer_started) {
                m_timer_started = true;
                uv_timer_init(s_uv_loop, this);
                uv_timer_start(this, on_timeout, m_timeout, 0);
            }
        }

        virtual ~UVTimeout()
        {
        }

        static void on_timeout(uv_timer_t* handle)
        {
            UVTimeout* pThis = (UVTimeout*)handle;
            pThis->on_timeout_handler();
        }

        // Default timeout handler: close the stream
        virtual void on_timeout_handler()
        {
            m_this->close(NULL);
        }

        static void on_timer_closed(uv_handle_t* handle)
        {
            delete (UVTimeout*)(uv_timer_t*)handle;
        }

        void cancel_timer()
        {
            if (m_timeout > 0 && m_timer_started) {
                uv_timer_stop(this);
                uv_close((uv_handle_t*)(uv_timer_t*)this, on_timer_closed);
            } else
                delete this;
        }

    public:
        obj_ptr<UVStream_tmpl> m_this;
        int32_t m_timeout;
        bool m_timer_started;
    };

public:
    UVStream_tmpl(int32_t fd = -1)
        : m_fd(fd)
    {
    }

public:
    static void on_delete(uv_handle_t* handle)
    {
        container_of(handle, UVStream_tmpl, m_handle)->safe_release();
    }

    virtual void Delete()
    {
        if (uv_is_closing(&m_handle)) {
            delete this;
            return;
        }

        uv_post([&] {
            uv_close(&m_handle, on_delete);
        });
    }

    static bool is_stdio_fd(int32_t fd)
    {
        return fd >= 0 && fd <= 2;
    }

public:
    // Stream_base
    class AsyncRead : public AsyncEvent,
                      public UVTimeout {
    public:
        AsyncRead(UVStream_tmpl* pThis, bool bRead, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
            : UVTimeout(pThis)
            , m_this(pThis)
            , m_bRead(bRead)
            , m_bytes(bytes)
            , m_retVal(retVal)
            , m_ac(ac)
            , m_pos(0)
        {
        }

        // Override timeout handler: remove self from queue and post timeout error
        virtual void on_timeout_handler() override
        {
            // Remove self from read queue
            m_this->queue_read.remove(this);

            // If queue is empty, stop reading
            if (m_this->queue_read.count() == 0)
                uv_read_stop(&m_this->m_stream);

            // Post timeout error
            post_result(CALL_E_TIMEOUT);
        }

    public:
        virtual void invoke()
        {
            UVTimeout::start_timer();  // Start timer in uv loop thread
            m_this->queue_read.putTail(this);
            if (m_this->queue_read.count() == 1) {
                int32_t ret = uv_read_start(&m_this->m_stream, on_alloc, on_read);
                if (ret < 0)
                    post_all_result(m_this, ret);
            }
        }

    public:
        static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
        {
            AsyncRead* ar = container_of(handle, UVStream_tmpl, m_handle)->queue_read.head();

            if (ar->m_buf.empty()) {
                if (ar->m_bytes > 0)
                    suggested_size = ar->m_bytes;
                else
                    suggested_size = STREAM_BLOCK_SIZE;

                ar->m_buf.resize(suggested_size);
            }

            buf->base = ar->m_buf.data() + ar->m_pos;
            buf->len = (int32_t)(ar->m_buf.length() - ar->m_pos);
        }

        static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
        {
            UVStream_tmpl* pThis = container_of(stream, UVStream_tmpl, m_handle);
            AsyncRead* ar;

            if (nread < 0) {
                uv_read_stop(&pThis->m_stream);
                post_all_result(pThis, (int32_t)nread);
                return;
            }

            ar = pThis->queue_read.head();
            ar->m_pos += nread;
            if (!ar->m_bRead || (ar->m_bytes < 0) || (ar->m_bytes == ar->m_pos)) {
                pThis->queue_read.getHead();
                ar->post_result(0);

                if (pThis->queue_read.count() == 0)
                    uv_read_stop(&pThis->m_stream);
            }
        }

        static void post_all_result(UVStream_tmpl* pThis, int32_t status)
        {
            while (pThis->queue_read.count())
                pThis->queue_read.getHead()->post_result(status);
        }

        void post_result(int32_t status)
        {
            if (status < 0 && status != UV_EOF && status != UV_ENOTCONN && status != UV_ECONNRESET) {
                m_ac->apost(status);
            } else {
                if (m_pos) {
                    if (m_pos < m_buf.length())
                        m_buf.resize(m_pos);

                    m_retVal = new Buffer(m_buf.c_str(), m_buf.length());

                    // Add pipedump output for named pipe read
                    if (g_pipedump && m_this->m_handle.type == UV_NAMED_PIPE) {
                        outLog(console_base::C_NOTICE, clean_string(m_buf.c_str(), m_buf.length()));
                    }

                    m_ac->apost(0);
                } else
                    m_ac->apost(CALL_RETURN_NULL);
            }
            UVTimeout::cancel_timer();
        }

    private:
        obj_ptr<UVStream_tmpl> m_this;
        bool m_bRead;
        int32_t m_bytes;
        obj_ptr<Buffer_base>& m_retVal;
        AsyncEvent* m_ac;
        size_t m_pos;
        exlib::string m_buf;
    };

    class AsyncWrite : public AsyncEvent,
                       public UVTimeout {
    public:
        AsyncWrite(UVStream_tmpl* pThis, Buffer_base* data, AsyncEvent* ac, int32_t timeout)
            : UVTimeout(pThis, timeout)
            , m_this(pThis)
            , m_ac(ac)
        {
            m_data = Buffer::Cast(data);
            m_buf.base = (char*)m_data->data();
            m_buf.len = (uint32_t)m_data->length();

            // Add pipedump output for named pipe write
            if (g_pipedump && pThis->m_handle.type == UV_NAMED_PIPE) {
                outLog(console_base::C_WARN, clean_string((char*)m_data->data(), m_data->length()));
            }
        }

        // Override timeout handler: remove self from queue and post timeout error
        virtual void on_timeout_handler() override
        {
            // Remove self from write queue
            m_this->queue_write.remove(this);

            // Post timeout error
            m_ac->apost(CALL_E_TIMEOUT);
            UVTimeout::cancel_timer();
        }

    public:
        virtual void invoke()
        {
            UVTimeout::start_timer();  // Start timer in uv loop thread
            m_this->queue_write.putTail(this);
            if (m_this->queue_write.count() == 1) {
                int32_t ret = uv_write(&m_req, &m_this->m_stream, &m_buf, 1, on_write);
                if (ret < 0)
                    post_all_result(m_this, ret);
            }
        }

    public:
        static void on_write(uv_write_t* req, int32_t status)
        {
            UVStream_tmpl* pThis = container_of(req->handle, UVStream_tmpl, m_handle);
            AsyncWrite* wr;

            if (status < 0) {
                post_all_result(pThis, status);
                return;
            }

            pThis->queue_write.getHead()->post_result(0);

            if (pThis->queue_write.count() > 0) {
                wr = pThis->queue_write.head();
                int32_t ret = uv_write(&wr->m_req, &pThis->m_stream, &wr->m_buf, 1, on_write);
                if (ret)
                    post_all_result(pThis, ret);
            }
        }

        static void post_all_result(UVStream_tmpl* pThis, int32_t status)
        {
            while (pThis->queue_write.count())
                pThis->queue_write.getHead()->post_result(status);
        }

        void post_result(int32_t status)
        {
            m_ac->apost(status);
            UVTimeout::cancel_timer();
        }

    private:
        obj_ptr<UVStream_tmpl> m_this;
        AsyncEvent* m_ac;
        obj_ptr<Buffer> m_data;
        uv_buf_t m_buf;
        uv_write_t m_req;
    };

    virtual result_t get_fd(int32_t& retVal)
    {
        if (is_stdio_fd(m_fd)) {
            retVal = m_fd;
            return 0;
        }

        uv_os_fd_t fd;
        result_t hr = uv_fileno(&m_handle, &fd);
        if (hr < 0)
            return hr;

        retVal = (int32_t)(intptr_t)fd;
        return 0;
    };

    virtual result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        uv_post(new AsyncRead(this, true, bytes, retVal, ac));
        return CALL_E_PENDDING;
    }

    virtual result_t writeBuffer(Buffer_base* data, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        uv_post(new AsyncWrite(this, data, ac, m_timeout));
        return CALL_E_PENDDING;
    }

    virtual result_t flush(AsyncEvent* ac)
    {
        return 0;
    }

    static void on_close(uv_handle_t* handle)
    {
        UVStream_tmpl* pThis = container_of(handle, UVStream_tmpl, m_handle);

        AsyncRead::post_all_result(pThis, UV_EPIPE);
        AsyncWrite::post_all_result(pThis, UV_EPIPE);

        pThis->on_handle_closed();

        if (pThis->ac_close)
            pThis->ac_close->apost(0);
    }

    virtual void on_handle_closed()
    {
    }

    virtual result_t close(AsyncEvent* ac)
    {
        if (ac && ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        uv_post([this, ac] {
            if (uv_is_closing(&this->m_handle)) {
                if (ac)
                    ac->apost(0);
                return;
            }

            this->ac_close = ac;
            uv_close(&this->m_handle, on_close);
        });

        return CALL_E_PENDDING;
    }

    virtual result_t get_timeout(int32_t& retVal)
    {
        retVal = m_timeout;
        return 0;
    }

    virtual result_t set_timeout(int32_t newVal)
    {
        m_timeout = newVal;
        return 0;
    }

    virtual result_t abort()
    {
        uv_post([this] {
            if (queue_read.count() > 0) {
                uv_read_stop(&m_stream);
                AsyncRead::post_all_result(this, CALL_E_ABORT);
            }

            if (queue_write.count() > 0) {
                AsyncWrite::post_all_result(this, CALL_E_ABORT);
            }
        });

        return 0;
    }

public:
    int32_t m_fd;
    int32_t m_timeout = -1;

public:
    union {
        uv_handle_t m_handle;
        uv_stream_t m_stream;
        uv_pipe_t m_pipe;
        uv_tty_t m_tty;
        uv_tcp_t m_tcp;
    };
    exlib::List<AsyncRead> queue_read;
    exlib::List<AsyncWrite> queue_write;
    AsyncEvent* ac_close;
};

class UVStream : public UVStream_tmpl<Stream_base> {
    FIBER_FREE();

public:
    UVStream(int32_t fd, int32_t ipc = 0)
        : UVStream_tmpl<Stream_base>(fd)
    {
        uv_call([&] {
            if (is_stdio_fd(fd) && uv_guess_handle(fd) == UV_TTY) {
                uv_tty_init(s_uv_loop, &m_tty, fd, 0);
                return uv_stream_set_blocking(&m_stream, 1);
            }

            uv_pipe_init(s_uv_loop, &m_pipe, ipc);
            return uv_pipe_open(&m_pipe, fd);
        });
    }

    ~UVStream()
    {
        if (m_on_close) {
            m_on_close(m_fd);
            m_on_close = nullptr;
        }
    }

public:
    virtual void on_handle_closed()
    {
        if (m_on_close) {
            m_on_close(m_fd);
            m_on_close = nullptr;
        }
    }

    virtual result_t onEventEmit(exlib::string ev)
    {
        if (ev == "close") {
            if (m_on_close) {
                m_on_close(m_fd);
                m_on_close = nullptr;
            }
        }

        return 0;
    }

public:
    static result_t create_pipe(obj_ptr<UVStream>& retVal, int32_t ipc, std::function<void(int32_t)> onClose)
    {
        obj_ptr<UVStream> stream = new UVStream(onClose);
        result_t hr = uv_call([&] {
            return uv_pipe_init(s_uv_loop, &stream->m_pipe, ipc);
        });
        if (hr < 0)
            return hr;

        retVal = stream;

        return 0;
    }

    static result_t uv_pipe(obj_ptr<UVStream>& retVal, int32_t fd, std::function<void(int32_t)> onClose)
    {
        obj_ptr<UVStream> stream = new UVStream(onClose);
        uv_pipe_init(s_uv_loop, &stream->m_pipe, 0);
        int ret = uv_pipe_open(&stream->m_pipe, fd);
        if (ret < 0)
            return ret;

        retVal = stream;

        return 0;
    }

private:
    UVStream(std::function<void(int32_t)> onClose)
        : m_on_close(onClose)
    {
    }

private:
    std::function<void(int32_t)> m_on_close;
};
}
