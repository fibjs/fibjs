/*
 * ZlibStream.h
 *
 *  Created on: Oct 26, 2017
 *      Author: lion
 */

#include "ifs/zlib.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <zlib/include/zlib.h>

#ifndef __ZLIBSTREAM_H_
#define __ZLIBSTREAM_H_

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

            set(process);
        }

        asyncWrite(ZlibStream* pThis, Stream_base* stm, int32_t flush, AsyncEvent* ac)
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

            if (pThis->m_pThis->strm.avail_out == ZLIB_CHUNK)
                return pThis->done();

            if (pThis->m_pThis->strm.avail_in == 0 && pThis->m_flush == Z_NO_FLUSH)
                return pThis->done();

            pThis->m_buffer = new Buffer((const char*)pThis->m_pThis->m_outBuffer,
                ZLIB_CHUNK - pThis->m_pThis->strm.avail_out);
            pThis->m_pThis->resetBuffer();
            return pThis->m_stm->write(pThis->m_buffer, pThis);
        }

    private:
        obj_ptr<ZlibStream> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<Buffer_base> m_buffer;
        exlib::string m_strBuf;
        int32_t m_flush;
    };

public:
    ZlibStream(Stream_base* stm)
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
        strm.avail_out = ZLIB_CHUNK;
        strm.next_out = m_outBuffer;
    }

    void attach(Stream_base* stm)
    {
        m_stm = stm;
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
            asyncProcess(ZlibStream* pThis, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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
    inf_base(Stream_base* stm)
        : ZlibStream(stm)
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

} /* namespace fibjs */
#endif /* __ZLIBSTREAM_H_ */
