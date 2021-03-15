/*
 * ZlibStream.h
 *
 *  Created on: Oct 26, 2017
 *      Author: lion
 */

#pragma once

#include "ifs/zlib.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <zlib/include/zlib.h>

namespace fibjs {

#define ZLIB_CHUNK 1024

class ZlibStream : public Stream_base {
private:
    class asyncWrite : public AsyncState {
    public:
        asyncWrite(ZlibStream* pThis, Stream_base* stm, Buffer_base* data, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(Z_NO_FLUSH)
        {
            data->toString(m_strBuf);
            pThis->strm.avail_in = (int32_t)m_strBuf.length();
            pThis->strm.next_in = (unsigned char*)m_strBuf.c_str();

            next(process);
        }

        asyncWrite(ZlibStream* pThis, Stream_base* stm, int32_t flush, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(flush)
        {
            pThis->strm.avail_in = 0;
            pThis->strm.next_in = (unsigned char*)m_strBuf.c_str();

            next(process);
        }

        ON_STATE(asyncWrite, process)
        {
            int32_t err;

            err = m_pThis->do_process(m_flush);
            if (err != Z_OK && err != Z_BUF_ERROR)
                return CHECK_ERROR(Runtime::setError(zError(err)));

            if (m_pThis->strm.avail_out == ZLIB_CHUNK)
                return next();

            if (m_pThis->strm.avail_in == 0 && m_flush == Z_NO_FLUSH)
                return next();

            int32_t size = ZLIB_CHUNK - m_pThis->strm.avail_out;
            if (m_pThis->m_maxSize >= 0) {
                if (size > m_pThis->m_maxSize - m_pThis->m_dataSize)
                    return CALL_E_OVERFLOW;
                m_pThis->m_dataSize += size;
            }

            m_buffer = new Buffer((const char*)m_pThis->m_outBuffer, size);
            m_pThis->resetBuffer();
            return m_stm->write(m_buffer, this);
        }

    private:
        obj_ptr<ZlibStream> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<Buffer_base> m_buffer;
        exlib::string m_strBuf;
        int32_t m_flush;
    };

public:
    ZlibStream(Stream_base* stm, int32_t maxSize = -1)
        : m_stm(stm)
        , m_maxSize(maxSize)
        , m_dataSize(0)
    {
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;

        resetBuffer();
    }

public:
    void resetBuffer()
    {
        strm.avail_out = ZLIB_CHUNK;
        strm.next_out = m_outBuffer;
    }

    void attach(Stream_base* stm)
    {
        m_stm = stm;
        m_dataSize = 0;
    }

public:
    // Stream_base
    result_t get_fd(int32_t& retVal)
    {
        return CALL_E_INVALID_CALL;
    }

    result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    result_t write(Buffer_base* data, AsyncEvent* ac)
    {
        return (new asyncWrite(this, m_stm, data, ac))->post(0);
    }

    result_t flush(AsyncEvent* ac)
    {
        return (new asyncWrite(this, m_stm, Z_SYNC_FLUSH, ac))->post(0);
    }

    result_t close(AsyncEvent* ac)
    {
        return (new asyncWrite(this, m_stm, Z_FINISH, ac))->post(0);
    }

    result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

public:
    result_t process(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(ZlibStream* pThis, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_data(data)
                , m_retVal(retVal)
            {
                m_stm = new MemoryStream();
                m_pThis->m_stm = m_stm;

                next(write);
            }

            ON_STATE(asyncProcess, write)
            {
                return m_pThis->write(m_data, next(write_ok));
            }

            ON_STATE(asyncProcess, write_ok)
            {
                return m_pThis->close(next(read));
            }

            ON_STATE(asyncProcess, read)
            {
                m_stm->rewind();
                return m_stm->readAll(m_retVal, next());
            }

        private:
            obj_ptr<ZlibStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
            obj_ptr<Buffer_base>& m_retVal;
            obj_ptr<MemoryStream> m_stm;
        };

        return (new asyncProcess(this, data, retVal, ac))->post(0);
    }

    result_t process(Buffer_base* data, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(ZlibStream* pThis, Buffer_base* data, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_data(data)
            {
                next(write);
            }

            ON_STATE(asyncProcess, write)
            {
                return m_pThis->write(m_data, next(write_ok));
            }

            ON_STATE(asyncProcess, write_ok)
            {
                return m_pThis->close(next());
            }

        private:
            obj_ptr<ZlibStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
        };

        return (new asyncProcess(this, data, ac))->post(0);
    }

    result_t process(Stream_base* src, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(ZlibStream* pThis, Stream_base* src, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_src(src)
            {
                next(copy);
            }

            ON_STATE(asyncProcess, copy)
            {
                return m_src->copyTo(m_pThis, -1, m_size, next(copy_ok));
            }

            ON_STATE(asyncProcess, copy_ok)
            {
                return m_pThis->close(next());
            }

        private:
            obj_ptr<ZlibStream> m_pThis;
            obj_ptr<Stream_base> m_src;
            int64_t m_size;
        };

        return (new asyncProcess(this, src, ac))->post(0);
    }

public:
    virtual int32_t do_process(int32_t flush)
    {
        return Z_OK;
    }

protected:
    z_stream strm;
    obj_ptr<Stream_base> m_stm;
    int32_t m_maxSize;
    int32_t m_dataSize;
    unsigned char m_outBuffer[ZLIB_CHUNK];
};

class def_base : public ZlibStream {
public:
    def_base(Stream_base* stm)
        : ZlibStream(stm)
    {
    }

    ~def_base()
    {
        deflateEnd(&strm);
    }

public:
    virtual int32_t do_process(int32_t flush)
    {
        ::deflate(&strm, flush);
        return Z_OK;
    }
};

class inf_base : public ZlibStream {
public:
    inf_base(Stream_base* stm, int32_t maxSize)
        : ZlibStream(stm, maxSize)
    {
    }

    ~inf_base()
    {
        inflateEnd(&strm);
    }

public:
    virtual int32_t do_process(int32_t flush)
    {
        int32_t ret = ::inflate(&strm, flush);

        if (ret == Z_DATA_ERROR) {
            ret = inflateSync(&strm);
            if (ret == Z_DATA_ERROR)
                ret = Z_STREAM_END;
        }

        if (ret == Z_STREAM_END) {
            inflateReset(&strm);
            return Z_OK;
        }

        return ret;
    }
};

class inf : public inf_base {
public:
    inf(Stream_base* stm, int32_t maxSize)
        : inf_base(stm, maxSize)
    {
        inflateInit(&strm);
    }
};

class def : public def_base {
public:
    def(Stream_base* stm, int32_t level = -1)
        : def_base(stm)
    {
        deflateInit(&strm, level);
    }
};

class gunz : public inf_base {
public:
    gunz(Stream_base* stm, int32_t maxSize)
        : inf_base(stm, maxSize)
    {
        inflateInit2(&strm, 15 + 16);
    }
};

class gz : public def_base {
public:
    gz(Stream_base* stm)
        : def_base(stm)
    {
        deflateInit2(&strm, -1, 8, 15 + 16, 8, 0);
    }
};

class infraw : public inf_base {
public:
    infraw(Stream_base* stm, int32_t maxSize)
        : inf_base(stm, maxSize)
    {
        inflateInit2(&strm, -15);
    }
};

class defraw : public def_base {
public:
    defraw(Stream_base* stm)
        : def_base(stm)
    {
        deflateInit2(&strm, -1, Z_DEFLATED, -15, 8, 0);
    }
};

} /* namespace fibjs */
