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
#include "AsyncStream.h"

namespace fibjs {

#define ZLIB_CHUNK 1024

// Window bits constants for different compression formats
#define GZIP_WINDOW_BITS (MAX_WBITS + 16) // 15 + 16 = 31 for GZIP format
#define RAW_WINDOW_BITS (-MAX_WBITS) // -15 for raw deflate format
#define ZLIB_WINDOW_BITS MAX_WBITS // 15 for standard zlib format
#define UNZIP_WINDOW_BITS (MAX_WBITS + 32) // 15 + 32 = 47 for auto-detect format

// Default compression parameters
#define DEFAULT_MEM_LEVEL 8 // Default memory level for deflate

// GZIP header magic numbers
#define GZIP_HEADER_ID1 0x1f
#define GZIP_HEADER_ID2 0x8b

class ZlibStream : public AsyncStream<Stream_base> {
private:
    class asyncWrite : public AsyncState {
    public:
        asyncWrite(ZlibStream* pThis, Stream_base* stm, Buffer_base* data, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(Z_NO_FLUSH)
        {
            m_data = Buffer::Cast(data);
            pThis->strm.avail_in = (int32_t)m_data->length();
            pThis->strm.next_in = (unsigned char*)m_data->data();

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
            return m_stm->write(m_buffer, m_len, this);
        }

    private:
        obj_ptr<ZlibStream> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<Buffer_base> m_buffer;
        obj_ptr<Buffer> m_data;
        exlib::string m_strBuf;
        int32_t m_flush;
        int32_t m_len;
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

    result_t write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
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
                return m_pThis->write(m_data, m_len, next(write_ok));
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
            int32_t m_len;
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
                return m_pThis->write(m_data, m_len, next(write_ok));
            }

            ON_STATE(asyncProcess, write_ok)
            {
                return m_pThis->close(next());
            }

        private:
            obj_ptr<ZlibStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
            int32_t m_len;
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
        , m_mode(INFLATE_MODE_UNKNOWN)
        , m_bytes_read(0)
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

        // Handle stream end normally
        if (ret == Z_STREAM_END) {
            inflateReset(&strm);
            return Z_OK;
        }

        // For any error, return it immediately without trying to recover
        // This ensures strict format validation and compatibility with Node.js
        if (ret != Z_OK && ret != Z_BUF_ERROR && ret != Z_NEED_DICT) {
            return ret;
        }

        return ret;
    }

protected:
    enum inflate_mode {
        INFLATE_MODE_UNKNOWN,
        INFLATE_MODE_GZIP,
        INFLATE_MODE_ZLIB,
        INFLATE_MODE_RAW
    };

    inflate_mode m_mode;
    int32_t m_bytes_read;
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
        if (level < zlib_base::C_DEFAULT_COMPRESSION)
            level = zlib_base::C_DEFAULT_COMPRESSION;
        else if (level > zlib_base::C_BEST_COMPRESSION)
            level = zlib_base::C_BEST_COMPRESSION;

        deflateInit(&strm, level);
    }
};

class gunz : public inf_base {
public:
    gunz(Stream_base* stm, int32_t maxSize)
        : inf_base(stm, maxSize)
    {
        inflateInit2(&strm, GZIP_WINDOW_BITS);
    }
};

class gz : public def_base {
public:
    gz(Stream_base* stm, int32_t level = -1)
        : def_base(stm)
    {
        if (level < zlib_base::C_DEFAULT_COMPRESSION)
            level = zlib_base::C_DEFAULT_COMPRESSION;
        else if (level > zlib_base::C_BEST_COMPRESSION)
            level = zlib_base::C_BEST_COMPRESSION;

        deflateInit2(&strm, level, Z_DEFLATED, GZIP_WINDOW_BITS, DEFAULT_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    }
};

class infraw : public inf_base {
public:
    infraw(Stream_base* stm, int32_t maxSize)
        : inf_base(stm, maxSize)
    {
        inflateInit2(&strm, RAW_WINDOW_BITS);
    }
};

class defraw : public def_base {
public:
    defraw(Stream_base* stm, int32_t level = -1)
        : def_base(stm)
    {
        if (level < zlib_base::C_DEFAULT_COMPRESSION)
            level = zlib_base::C_DEFAULT_COMPRESSION;
        else if (level > zlib_base::C_BEST_COMPRESSION)
            level = zlib_base::C_BEST_COMPRESSION;

        deflateInit2(&strm, level, Z_DEFLATED, RAW_WINDOW_BITS, DEFAULT_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    }
};

class zip : public def_base {
public:
    zip(Stream_base* stm, int32_t level = -1)
        : def_base(stm)
    {
        if (level < zlib_base::C_DEFAULT_COMPRESSION)
            level = zlib_base::C_DEFAULT_COMPRESSION;
        else if (level > zlib_base::C_BEST_COMPRESSION)
            level = zlib_base::C_BEST_COMPRESSION;

        // Use standard deflate format (with zlib headers) for Node.js compatibility
        // This allows fibjs zip data to be decompressed by Node.js unzipSync
        deflateInit2(&strm, level, Z_DEFLATED, ZLIB_WINDOW_BITS, DEFAULT_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    }
};

class unzip : public inf_base {
public:
    unzip(Stream_base* stm, int32_t maxSize)
        : inf_base(stm, maxSize)
        , m_auto_detect_init(false)
    {
        // Don't initialize zlib here - we'll do it after format detection
    }

public:
    virtual int32_t do_process(int32_t flush) override
    {
        // Auto-detect format on first call
        if (!m_auto_detect_init && strm.avail_in > 0) {
            if (!detect_and_init_format()) {
                return Z_DATA_ERROR;
            }
            m_auto_detect_init = true;
        }

        // If we still don't have format, return error
        if (!m_auto_detect_init) {
            return Z_DATA_ERROR;
        }

        return inf_base::do_process(flush);
    }

private:
    bool detect_and_init_format()
    {
        if (strm.avail_in < 1) {
            // Need at least 1 byte for format detection
            return false;
        }

        unsigned char* data = strm.next_in;

        // Check for GZIP format (0x1f, 0x8b)
        if (data[0] == GZIP_HEADER_ID1 && strm.avail_in >= 2 && data[1] == GZIP_HEADER_ID2) {
            // For GZIP format, we need at least the minimum header size (10 bytes)
            // to ensure it's a complete gzip header, not just the magic number
            if (strm.avail_in < 10) {
                // Incomplete gzip header - need more data
                // But since this is sync operation, we don't have more data coming
                // So we should reject incomplete headers to match Node.js behavior
                return false;
            }
            m_mode = INFLATE_MODE_GZIP;
            return inflateInit2(&strm, GZIP_WINDOW_BITS) == Z_OK;
        }

        // Check for zlib/deflate format (0x78, 0x9c or other valid combinations)
        if (data[0] == 0x78) {
            // For zlib format, we need at least 2 bytes for the header
            if (strm.avail_in < 2) {
                return false;
            }
            m_mode = INFLATE_MODE_ZLIB;
            return inflateInit2(&strm, ZLIB_WINDOW_BITS) == Z_OK;
        }

        // For Node.js compatibility, unzip should NOT handle deflateRaw format
        // Only support gzip and deflate (zlib) formats like Node.js unzipSync
        return false;
    }

private:
    bool m_auto_detect_init;
};

} /* namespace fibjs */
