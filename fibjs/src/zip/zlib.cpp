/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/zlib.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "StringBuffer.h"
#include <zlib/include/zlib.h>
#include <sstream>

#define CHUNK 1024

namespace fibjs {

DECLARE_MODULE(zlib);

class zlibWorker : public Stream_base {
private:
    class asyncWrite : public AsyncState {
    public:
        asyncWrite(zlibWorker* pThis, Stream_base* stm, Buffer_base* data, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(Z_NO_FLUSH)
        {
            data->toString(m_strBuf);
            pThis->strm.avail_in = (int32_t)m_strBuf.length();
            pThis->strm.next_in = (unsigned char*)m_strBuf.c_str();

            set(process);
        }

        asyncWrite(zlibWorker* pThis, Stream_base* stm, int32_t flush, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(flush)
        {
            pThis->strm.avail_in = 0;
            pThis->strm.next_in = (unsigned char*)m_strBuf.c_str();

            set(process);
        }

        static int32_t process(AsyncState* pState, int32_t n)
        {
            asyncWrite* pThis = (asyncWrite*)pState;
            int32_t err;

            err = pThis->m_pThis->do_process(pThis->m_flush);
            if (err != Z_OK && err != Z_BUF_ERROR)
                return CHECK_ERROR(Runtime::setError(zError(err)));

            if (pThis->m_pThis->strm.avail_out == CHUNK)
                return pThis->done();

            if (pThis->m_pThis->strm.avail_in == 0 && pThis->m_flush == Z_NO_FLUSH)
                return pThis->done();

            pThis->m_buffer = new Buffer((const char*)pThis->m_pThis->m_outBuffer,
                CHUNK - pThis->m_pThis->strm.avail_out);
            pThis->m_pThis->resetBuffer();
            return pThis->m_stm->write(pThis->m_buffer, pThis);
        }

    private:
        obj_ptr<zlibWorker> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<Buffer_base> m_buffer;
        exlib::string m_strBuf;
        int32_t m_flush;
    };

public:
    zlibWorker(Stream_base* stm)
        : m_stm(stm)
    {
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;

        resetBuffer();
    }

public:
    void resetBuffer()
    {
        strm.avail_out = CHUNK;
        strm.next_out = m_outBuffer;
    }

public:
    // Stream_base
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
            asyncProcess(zlibWorker* pThis, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_data(data)
                , m_retVal(retVal)
            {
                m_stm = new MemoryStream();
                m_pThis->m_stm = m_stm;

                set(write);
            }

            static int32_t write(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->set(write_ok);
                return pThis->m_pThis->write(pThis->m_data, pThis);
            }

            static int32_t write_ok(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->set(read);
                return pThis->m_pThis->close(pThis);
            }

            static int32_t read(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->m_stm->rewind();
                pThis->done();
                return pThis->m_stm->readAll(pThis->m_retVal, pThis);
            }

        private:
            obj_ptr<zlibWorker> m_pThis;
            obj_ptr<Buffer_base> m_data;
            obj_ptr<Buffer_base>& m_retVal;
            obj_ptr<MemoryStream> m_stm;
            int64_t m_size;
        };

        return (new asyncProcess(this, data, retVal, ac))->post(0);
    }

    result_t process(Buffer_base* data, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(zlibWorker* pThis, Buffer_base* data, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_data(data)
            {
                set(write);
            }

            static int32_t write(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->set(write_ok);
                return pThis->m_pThis->write(pThis->m_data, pThis);
            }

            static int32_t write_ok(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->done();
                return pThis->m_pThis->close(pThis);
            }

        private:
            obj_ptr<zlibWorker> m_pThis;
            obj_ptr<Buffer_base> m_data;
            int64_t m_size;
        };

        return (new asyncProcess(this, data, ac))->post(0);
    }

    result_t process(Stream_base* src, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(zlibWorker* pThis, Stream_base* src, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_src(src)
            {
                set(copy);
            }

            static int32_t copy(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->set(copy_ok);
                return pThis->m_src->copyTo(pThis->m_pThis, -1, pThis->m_size, pThis);
            }

            static int32_t copy_ok(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->done();
                return pThis->m_pThis->close(pThis);
            }

        private:
            obj_ptr<zlibWorker> m_pThis;
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
    unsigned char m_outBuffer[CHUNK];
};

class def_base : public zlibWorker {
public:
    def_base(Stream_base* stm)
        : zlibWorker(stm)
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

class inf_base : public zlibWorker {
public:
    inf_base(Stream_base* stm)
        : zlibWorker(stm)
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
        if (ret == Z_STREAM_END) {
            inflateReset(&strm);
            return Z_OK;
        }
        if (ret == Z_DATA_ERROR)
            ret = inflateSync(&strm);

        return ret;
    }
};

class inf : public inf_base {
public:
    inf(Stream_base* stm)
        : inf_base(stm)
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
    gunz(Stream_base* stm)
        : inf_base(stm)
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
    infraw(Stream_base* stm)
        : inf_base(stm)
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

result_t zlib_base::createDeflate(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new def(to, -1);
    return 0;
}

result_t zlib_base::createDeflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new defraw(to);
    return 0;
}

result_t zlib_base::createGunzip(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new gunz(to);
    return 0;
}

result_t zlib_base::createGzip(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new gz(to);
    return 0;
}

result_t zlib_base::createInflate(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new inf(to);
    return 0;
}

result_t zlib_base::createInflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new infraw(to);
    return 0;
}

result_t zlib_base::deflate(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(NULL, level))->process(data, retVal, ac);
}

result_t zlib_base::deflateTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(stm, level))->process(data, ac);
}

result_t zlib_base::deflateTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(stm, level))->process(src, ac);
}

result_t zlib_base::inflate(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(NULL))->process(data, retVal, ac);
}

result_t zlib_base::inflateTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(stm))->process(data, ac);
}

result_t zlib_base::inflateTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(stm))->process(src, ac);
}

result_t zlib_base::gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(NULL))->process(data, retVal, ac);
}

result_t zlib_base::gzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(stm))->process(data, ac);
}

result_t zlib_base::gzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(stm))->process(src, ac);
}

result_t zlib_base::gunzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(NULL))->process(data, retVal, ac);
}

result_t zlib_base::gunzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(stm))->process(data, ac);
}

result_t zlib_base::gunzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(stm))->process(src, ac);
}

result_t zlib_base::deflateRaw(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(NULL))->process(data, retVal, ac);
}

result_t zlib_base::deflateRawTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(stm))->process(data, ac);
}

result_t zlib_base::deflateRawTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(stm))->process(src, ac);
}

result_t zlib_base::inflateRaw(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(NULL))->process(data, retVal, ac);
}

result_t zlib_base::inflateRawTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(stm))->process(data, ac);
}

result_t zlib_base::inflateRawTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(stm))->process(src, ac);
}
}
