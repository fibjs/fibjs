/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "ifs/zlib.h"
#include "Buffer.h"
#include "zlib.h"
#include <sstream>

#define CHUNK 32768

namespace fibjs
{

class zlibWorker
{
public:
    zlibWorker()
    {
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
    }

public:
    result_t process(Buffer_base *data, obj_ptr<Buffer_base> &retVal)
    {
        unsigned char out[CHUNK];
        std::string strBuf;
        std::stringstream outBuf;

        if (init() != Z_OK)
            return CHECK_ERROR(CALL_E_OVERFLOW);

        data->toString(strBuf);

        strm.avail_in = (int) strBuf.length();
        strm.next_in = (unsigned char *) strBuf.c_str();

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            if (put() != Z_OK)
            {
                end();
                return CHECK_ERROR(CALL_E_INVALID_DATA);
            }

            outBuf.write((const char *) out, CHUNK - strm.avail_out);
        }
        while (strm.avail_out == 0 || !fin());

        end();

        retVal = new Buffer(outBuf.str());

        return 0;
    }

    result_t process(Buffer_base *data, Stream_base *stm, exlib::AsyncEvent *ac)
    {
        class asyncProcess: public asyncState
        {
        public:
            asyncProcess(zlibWorker *pThis, Stream_base *stm,
                         exlib::AsyncEvent *ac) :
                asyncState(ac), m_pThis(pThis), m_stm(stm)
            {
                set(process);
            }

            static int process(asyncState *pState, int n)
            {
                asyncProcess *pThis = (asyncProcess *) pState;

                if (pThis->m_pThis->strm.avail_out != 0
                        && pThis->m_pThis->fin())
                {
                    pThis->m_pThis->end();
                    return pThis->done();
                }

                pThis->m_pThis->strm.avail_out = CHUNK;
                pThis->m_pThis->strm.next_out = pThis->out;

                if (pThis->m_pThis->put() != Z_OK)
                    return CHECK_ERROR(CALL_E_INVALID_DATA);

                pThis->m_buffer = new Buffer(
                    std::string((const char *) pThis->out,
                                CHUNK - pThis->m_pThis->strm.avail_out));

                return pThis->m_stm->write(pThis->m_buffer, pThis);
            }

            virtual int error(int v)
            {
                m_pThis->end();
                return v;
            }

        private:
            zlibWorker *m_pThis;
            obj_ptr<Stream_base> m_stm;
            unsigned char out[CHUNK];
            obj_ptr<Buffer_base> m_buffer;
        };

        if (init() != Z_OK)
            return CHECK_ERROR(CALL_E_OVERFLOW);

        std::string strBuf;

        data->toString(strBuf);

        strm.avail_in = (int) strBuf.length();
        strm.next_in = (unsigned char *) strBuf.c_str();
        strm.avail_out = 0;

        return (new asyncProcess(this, stm, ac))->post(0);
    }

    result_t process(Stream_base *src, Stream_base *stm, exlib::AsyncEvent *ac)
    {
        class asyncProcess: public asyncState
        {
        public:
            asyncProcess(zlibWorker *pThis, Stream_base *src, Stream_base *stm,
                         exlib::AsyncEvent *ac) :
                asyncState(ac), m_pThis(pThis), m_src(src), m_stm(stm)
            {
                set(read);
            }

            static int read(asyncState *pState, int n)
            {
                asyncProcess *pThis = (asyncProcess *) pState;

                pThis->set(read_ok);
                return pThis->m_src->read(-1, pThis->m_buffer, pThis);
            }

            static int read_ok(asyncState *pState, int n)
            {
                asyncProcess *pThis = (asyncProcess *) pState;

                if (n != CALL_RETURN_NULL)
                {
                    pThis->m_buffer->toString(pThis->m_strBuf);
                    pThis->m_pThis->strm.avail_in =
                        (int) pThis->m_strBuf.length();
                    pThis->m_pThis->strm.next_in =
                        (unsigned char *) pThis->m_strBuf.c_str();
                    pThis->m_pThis->strm.avail_out = 0;
                }
                else if (pThis->m_pThis->fin())
                {
                    pThis->set(end);
                    return 0;
                }

                pThis->set(process);
                return 0;
            }

            static int process(asyncState *pState, int n)
            {
                asyncProcess *pThis = (asyncProcess *) pState;

                pThis->m_pThis->strm.avail_out = CHUNK;
                pThis->m_pThis->strm.next_out = pThis->out;

                if (pThis->m_pThis->put() != Z_OK)
                    return CHECK_ERROR(CALL_E_INVALID_DATA);

                pThis->m_buffer = new Buffer(
                    std::string((const char *) pThis->out,
                                CHUNK - pThis->m_pThis->strm.avail_out));

                pThis->set(write_ok);
                return pThis->m_stm->write(pThis->m_buffer, pThis);
            }

            static int write_ok(asyncState *pState, int n)
            {
                asyncProcess *pThis = (asyncProcess *) pState;

                if (pThis->m_pThis->strm.avail_out != 0)
                {
                    pThis->set(read);
                    return 0;
                }

                pThis->set(process);
                return 0;
            }

            static int end(asyncState *pState, int n)
            {
                asyncProcess *pThis = (asyncProcess *) pState;

                pThis->m_pThis->end();
                return pThis->done();
            }

            virtual int error(int v)
            {
                m_pThis->end();
                return v;
            }

        private:
            zlibWorker *m_pThis;
            obj_ptr<Stream_base> m_src;
            obj_ptr<Stream_base> m_stm;
            unsigned char out[CHUNK];
            obj_ptr<Buffer_base> m_buffer;
            std::string m_strBuf;
        };

        if (init() != Z_OK)
            return CHECK_ERROR(CALL_E_OVERFLOW);

        return (new asyncProcess(this, src, stm, ac))->post(0);
    }

public:
    virtual int init()
    {
        return Z_OK;
    }

    virtual int put()
    {
        return Z_OK;
    }

    virtual bool fin()
    {
        return true;
    }

    virtual void end()
    {
    }

protected:
    z_stream strm;
};

class def: public zlibWorker
{
public:
    def(int level = -1) :
        m_level(level), flush(Z_NO_FLUSH)
    {
    }

public:
    virtual int init()
    {
        return deflateInit(&strm, m_level);
    }

    virtual int put()
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
    }

private:
    int m_level;
    int flush;
};

class inf: public zlibWorker
{
public:
    virtual int init()
    {
        return inflateInit(&strm);
    }

    virtual int put()
    {
        int ret = ::inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_END)
        {
            inflateReset(&strm);
            return Z_OK;
        }
        return ret;
    }

    virtual void end()
    {
        inflateEnd(&strm);
    }
};

class gunz: public inf
{
public:
    virtual int init()
    {
        return inflateInit2(&strm, 15 + 16);
    }
};

class gz: public def
{
public:
    virtual int init()
    {
        return deflateInit2(&strm, -1, 8, 15 + 16, 8, 0);
    }
};

result_t zlib_base::deflate(Buffer_base *data, int32_t level,
                            obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return def(level).process(data, retVal);
}

result_t zlib_base::deflateTo(Buffer_base *data, Stream_base *stm,
                              int32_t level, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return def(level).process(data, stm, ac);
}

result_t zlib_base::deflateTo(Stream_base *src, Stream_base *stm, int32_t level,
                              exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return def(level).process(src, stm, ac);
}

result_t zlib_base::inflate(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                            exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return inf().process(data, retVal);
}

result_t zlib_base::inflateTo(Buffer_base *data, Stream_base *stm,
                              exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return inf().process(data, stm, ac);
}

result_t zlib_base::inflateTo(Stream_base *src, Stream_base *stm,
                              exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return inf().process(src, stm, ac);
}

result_t zlib_base::gzip(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                         exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return gz().process(data, retVal);
}

result_t zlib_base::gzipTo(Buffer_base *data, Stream_base *stm,
                           exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return gz().process(data, stm, ac);
}

result_t zlib_base::gzipTo(Stream_base *src, Stream_base *stm,
                           exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return gz().process(src, stm, ac);
}

result_t zlib_base::gunzip(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                           exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return gunz().process(data, retVal);
}

result_t zlib_base::gunzipTo(Buffer_base *data, Stream_base *stm,
                             exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return gunz().process(data, stm, ac);
}

result_t zlib_base::gunzipTo(Stream_base *src, Stream_base *stm,
                             exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return gunz().process(src, stm, ac);
}

}

