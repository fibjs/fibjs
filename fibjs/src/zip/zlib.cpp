/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/zlib.h"
#include "Buffer.h"
#include "StringBuffer.h"
#include <zlib/include/zlib.h>
#include <sstream>

#define CHUNK 32768

namespace fibjs {

DECLARE_MODULE(zlib);

class zlibWorker {
public:
    zlibWorker()
    {
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
    }

    virtual ~zlibWorker()
    {
    }

public:
    result_t process(Buffer_base* data, obj_ptr<Buffer_base>& retVal)
    {
        unsigned char out[CHUNK];
        exlib::string strBuf;
        StringBuffer outBuf;
        int32_t err;

        err = init();
        if (err != Z_OK)
            return CHECK_ERROR(Runtime::setError(zError(err)));

        data->toString(strBuf);

        strm.avail_in = (int32_t)strBuf.length();
        strm.next_in = (unsigned char*)strBuf.c_str();

        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            err = put();
            if (err != Z_OK && err != Z_BUF_ERROR) {
                end();
                return CHECK_ERROR(Runtime::setError(zError(err)));
            }

            outBuf.append((const char*)out, CHUNK - strm.avail_out);
        } while (strm.avail_out == 0 || !fin());

        end();

        retVal = new Buffer(outBuf.str());

        return 0;
    }

    result_t process(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(zlibWorker* pThis, Stream_base* stm,
                AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_stm(stm)
            {
                set(process);
            }

            static int32_t process(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;
                int32_t err;

                if (pThis->m_pThis->strm.avail_out != 0
                    && pThis->m_pThis->fin()) {
                    pThis->m_pThis->end();
                    return pThis->done();
                }

                pThis->m_pThis->strm.avail_out = CHUNK;
                pThis->m_pThis->strm.next_out = pThis->out;

                err = pThis->m_pThis->put();
                if (err != Z_OK && err != Z_BUF_ERROR)
                    return CHECK_ERROR(Runtime::setError(zError(err)));

                pThis->m_buffer = new Buffer(
                    (const char*)pThis->out,
                    CHUNK - pThis->m_pThis->strm.avail_out);

                return pThis->m_stm->write(pThis->m_buffer, pThis);
            }

            virtual int32_t error(int32_t v)
            {
                m_pThis->end();
                return v;
            }

        private:
            zlibWorker* m_pThis;
            obj_ptr<Stream_base> m_stm;
            unsigned char out[CHUNK];
            obj_ptr<Buffer_base> m_buffer;
        };

        int32_t err;

        err = init();
        if (err != Z_OK)
            return CHECK_ERROR(Runtime::setError(zError(err)));

        exlib::string strBuf;

        data->toString(strBuf);

        strm.avail_in = (int32_t)strBuf.length();
        strm.next_in = (unsigned char*)strBuf.c_str();
        strm.avail_out = 0;

        return (new asyncProcess(this, stm, ac))->post(0);
    }

    result_t process(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(zlibWorker* pThis, Stream_base* src, Stream_base* stm,
                AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_src(src)
                , m_stm(stm)
            {
                set(read);
            }

            static int32_t read(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->set(read_ok);
                return pThis->m_src->read(-1, pThis->m_buffer, pThis);
            }

            static int32_t read_ok(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                if (n != CALL_RETURN_NULL) {
                    pThis->m_buffer->toString(pThis->m_strBuf);
                    pThis->m_pThis->strm.avail_in = (int32_t)pThis->m_strBuf.length();
                    pThis->m_pThis->strm.next_in = (unsigned char*)pThis->m_strBuf.c_str();
                    pThis->m_pThis->strm.avail_out = 0;
                } else if (pThis->m_pThis->fin()) {
                    pThis->set(end);
                    return 0;
                }

                pThis->set(process);
                return 0;
            }

            static int32_t process(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;
                int32_t err;

                pThis->m_pThis->strm.avail_out = CHUNK;
                pThis->m_pThis->strm.next_out = pThis->out;

                err = pThis->m_pThis->put();
                if (err != Z_OK && err != Z_BUF_ERROR)
                    return CHECK_ERROR(Runtime::setError(zError(err)));

                pThis->m_buffer = new Buffer(
                    (const char*)pThis->out,
                    CHUNK - pThis->m_pThis->strm.avail_out);

                pThis->set(write_ok);
                return pThis->m_stm->write(pThis->m_buffer, pThis);
            }

            static int32_t write_ok(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                if (pThis->m_pThis->strm.avail_out != 0) {
                    pThis->set(read);
                    return 0;
                }

                pThis->set(process);
                return 0;
            }

            static int32_t end(AsyncState* pState, int32_t n)
            {
                asyncProcess* pThis = (asyncProcess*)pState;

                pThis->m_pThis->end();
                return pThis->done();
            }

            virtual int32_t error(int32_t v)
            {
                m_pThis->end();
                return v;
            }

        private:
            zlibWorker* m_pThis;
            obj_ptr<Stream_base> m_src;
            obj_ptr<Stream_base> m_stm;
            unsigned char out[CHUNK];
            obj_ptr<Buffer_base> m_buffer;
            exlib::string m_strBuf;
        };

        int32_t err;

        err = init();
        if (err != Z_OK)
            return CHECK_ERROR(Runtime::setError(zError(err)));

        return (new asyncProcess(this, src, stm, ac))->post(0);
    }

public:
    virtual int32_t init()
    {
        return Z_OK;
    }

    virtual int32_t put()
    {
        return Z_OK;
    }

    virtual bool fin()
    {
        return true;
    }

    virtual void end()
    {
        delete this;
    }

protected:
    z_stream strm;
};

class def : public zlibWorker {
public:
    def(int32_t level = -1)
        : m_level(level)
        , flush(Z_NO_FLUSH)
    {
    }

public:
    virtual int32_t init()
    {
        return deflateInit(&strm, m_level);
    }

    virtual int32_t put()
    {
        ::deflate(&strm, flush);
        return Z_OK;
    }

    virtual bool fin()
    {
        if (flush == Z_FINISH)
            return true;

        flush = Z_FINISH;
        return false;
    }

    virtual void end()
    {
        deflateEnd(&strm);
        zlibWorker::end();
    }

private:
    int32_t m_level;
    int32_t flush;
};

class inf : public zlibWorker {
public:
    virtual int32_t init()
    {
        return inflateInit(&strm);
    }

    virtual int32_t put()
    {
        int32_t ret = ::inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_END) {
            inflateReset(&strm);
            return Z_OK;
        }
        if (ret == Z_DATA_ERROR) {
            ret = inflateSync(&strm);
        }
        return ret;
    }

    virtual void end()
    {
        inflateEnd(&strm);
        zlibWorker::end();
    }
};

class gunz : public inf {
public:
    virtual int32_t init()
    {
        return inflateInit2(&strm, 15 + 16);
    }
};

class gz : public def {
public:
    virtual int32_t init()
    {
        return deflateInit2(&strm, -1, 8, 15 + 16, 8, 0);
    }
};

class infraw : public inf {
public:
    virtual int32_t init()
    {
        return inflateInit2(&strm, -15);
    }
};

class defraw : public def {
public:
    virtual int32_t init()
    {
        return deflateInit2(&strm, -1, Z_DEFLATED, -15, 8, 0);
    }
};

result_t zlib_base::deflate(Buffer_base* data, int32_t level,
    obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(level))->process(data, retVal);
}

result_t zlib_base::deflateTo(Buffer_base* data, Stream_base* stm,
    int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(level))->process(data, stm, ac);
}

result_t zlib_base::deflateTo(Stream_base* src, Stream_base* stm, int32_t level,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(level))->process(src, stm, ac);
}

result_t zlib_base::inflate(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf())->process(data, retVal);
}

result_t zlib_base::inflateTo(Buffer_base* data, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf())->process(data, stm, ac);
}

result_t zlib_base::inflateTo(Stream_base* src, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf())->process(src, stm, ac);
}

result_t zlib_base::gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz())->process(data, retVal);
}

result_t zlib_base::gzipTo(Buffer_base* data, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz())->process(data, stm, ac);
}

result_t zlib_base::gzipTo(Stream_base* src, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz())->process(src, stm, ac);
}

result_t zlib_base::gunzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz())->process(data, retVal);
}

result_t zlib_base::gunzipTo(Buffer_base* data, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz())->process(data, stm, ac);
}

result_t zlib_base::gunzipTo(Stream_base* src, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz())->process(src, stm, ac);
}

result_t zlib_base::deflateRaw(Buffer_base* data, int32_t level,
    obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw())->process(data, retVal);
}

result_t zlib_base::deflateRawTo(Buffer_base* data, Stream_base* stm,
    int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw())->process(data, stm, ac);
}

result_t zlib_base::deflateRawTo(Stream_base* src, Stream_base* stm, int32_t level,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw())->process(src, stm, ac);
}

result_t zlib_base::inflateRaw(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw())->process(data, retVal);
}

result_t zlib_base::inflateRawTo(Buffer_base* data, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw())->process(data, stm, ac);
}

result_t zlib_base::inflateRawTo(Stream_base* src, Stream_base* stm,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw())->process(src, stm, ac);
}
}
