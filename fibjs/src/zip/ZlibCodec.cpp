/*
 * ZlibCodec.cpp
 *
 * Node.js compatible zlib class constructors for use with minizlib/tar
 */

#include "object.h"
#include "ifs/Gzip.h"
#include "ifs/Gunzip.h"
#include "ifs/Deflate.h"
#include "ifs/Inflate.h"
#include "ifs/DeflateRaw.h"
#include "ifs/InflateRaw.h"
#include "ifs/Unzip.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <zlib/include/zlib.h>

namespace fibjs {

enum ZlibMode {
    ZLIB_GZIP,
    ZLIB_GUNZIP,
    ZLIB_DEFLATE,
    ZLIB_INFLATE,
    ZLIB_DEFLATERAW,
    ZLIB_INFLATERAW,
    ZLIB_UNZIP
};

#define ZLIB_CODEC_CHUNK 16384

// Window bits constants
#define CODEC_GZIP_WINDOW_BITS (MAX_WBITS + 16)
#define CODEC_RAW_WINDOW_BITS (-MAX_WBITS)
#define CODEC_ZLIB_WINDOW_BITS MAX_WBITS
#define CODEC_UNZIP_WINDOW_BITS (MAX_WBITS + 32)

template <typename BASE>
class ZlibCodecImpl : public BASE {
public:
    ZlibCodecImpl(ZlibMode mode, int32_t level, int32_t windowBits, int32_t memLevel, int32_t strategy)
        : m_mode(mode)
        , m_closed(false)
    {
        memset(&m_strm, 0, sizeof(m_strm));
        m_strm.zalloc = Z_NULL;
        m_strm.zfree = Z_NULL;
        m_strm.opaque = Z_NULL;

        int ret;
        if (is_deflate()) {
            ret = deflateInit2(&m_strm, level, Z_DEFLATED, windowBits, memLevel, strategy);
        } else {
            ret = inflateInit2(&m_strm, windowBits);
        }

        if (ret != Z_OK)
            m_closed = true;
    }

    ~ZlibCodecImpl()
    {
        cleanup();
    }

    // ZlibCodec_base
    virtual result_t _processChunk(Buffer_base* chunk, int32_t flushFlag, obj_ptr<Buffer_base>& retVal) override
    {
        if (m_closed)
            return CHECK_ERROR(Runtime::setError("zlib binding closed"));

        obj_ptr<Buffer> input = Buffer::Cast(chunk);
        m_strm.avail_in = (uInt)input->length();
        m_strm.next_in = (Bytef*)input->data();

        std::vector<uint8_t> output;

        do {
            uint8_t outbuf[ZLIB_CODEC_CHUNK];
            m_strm.avail_out = ZLIB_CODEC_CHUNK;
            m_strm.next_out = outbuf;

            int ret;
            if (is_deflate())
                ret = ::deflate(&m_strm, flushFlag);
            else
                ret = ::inflate(&m_strm, flushFlag);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                const char* msg = m_strm.msg ? m_strm.msg : zError(ret);
                return CHECK_ERROR(Runtime::setError(msg));
            }

            uInt have = ZLIB_CODEC_CHUNK - m_strm.avail_out;
            if (have > 0)
                output.insert(output.end(), outbuf, outbuf + have);

            if (ret == Z_STREAM_END) {
                if (is_deflate())
                    deflateReset(&m_strm);
                else
                    inflateReset(&m_strm);
                break;
            }
        } while (m_strm.avail_out == 0);

        if (output.empty())
            retVal = new Buffer(NULL, 0);
        else
            retVal = new Buffer((const char*)output.data(), output.size());

        return 0;
    }

    virtual result_t close() override
    {
        cleanup();
        m_closed = true;
        return 0;
    }

    virtual result_t reset() override
    {
        if (m_closed)
            return 0;

        if (is_deflate())
            deflateReset(&m_strm);
        else
            inflateReset(&m_strm);

        return 0;
    }

    virtual result_t get__handle(v8::Local<v8::Value>& retVal) override
    {
        Isolate* isolate = this->holder();
        v8::Local<v8::Context> context = isolate->context();

        if (m_handle.IsEmpty()) {
            // Create the _handle object with a close method
            v8::Local<v8::Object> handle = v8::Object::New(isolate->m_isolate);
            v8::Local<v8::Function> closeFn = v8::Function::New(context,
                [](const v8::FunctionCallbackInfo<v8::Value>& info) {
                    // no-op close for compatibility
                })
                                                  .ToLocalChecked();
            handle->Set(context, isolate->NewString("close"), closeFn).Check();
            m_handle.Reset(isolate->m_isolate, handle);
        }

        retVal = m_handle.Get(isolate->m_isolate);
        return 0;
    }

    virtual result_t set__handle(v8::Local<v8::Value> newVal) override
    {
        Isolate* isolate = this->holder();
        m_handle.Reset(isolate->m_isolate, newVal);
        return 0;
    }

    virtual result_t params(int32_t level, int32_t strategy) override
    {
        if (m_closed)
            return CHECK_ERROR(Runtime::setError("zlib binding closed"));

        if (!is_deflate())
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        deflateParams(&m_strm, level, strategy);
        return 0;
    }

private:
    bool is_deflate() const
    {
        return m_mode == ZLIB_GZIP || m_mode == ZLIB_DEFLATE || m_mode == ZLIB_DEFLATERAW;
    }

    void cleanup()
    {
        if (!m_closed) {
            if (is_deflate())
                deflateEnd(&m_strm);
            else
                inflateEnd(&m_strm);
        }
    }

private:
    ZlibMode m_mode;
    z_stream m_strm;
    bool m_closed;
    v8::Global<v8::Value> m_handle;
};

// Helper to extract options from opts object
static void extract_opts(v8::Local<v8::Object> opts, ZlibMode mode,
    int32_t& level, int32_t& windowBits, int32_t& memLevel, int32_t& strategy)
{
    level = Z_DEFAULT_COMPRESSION;
    memLevel = 8;
    strategy = Z_DEFAULT_STRATEGY;

    // Set default windowBits based on mode
    switch (mode) {
    case ZLIB_GZIP:
        windowBits = CODEC_GZIP_WINDOW_BITS;
        break;
    case ZLIB_GUNZIP:
        windowBits = CODEC_GZIP_WINDOW_BITS;
        break;
    case ZLIB_DEFLATE:
        windowBits = CODEC_ZLIB_WINDOW_BITS;
        break;
    case ZLIB_INFLATE:
        windowBits = CODEC_ZLIB_WINDOW_BITS;
        break;
    case ZLIB_DEFLATERAW:
        windowBits = CODEC_RAW_WINDOW_BITS;
        break;
    case ZLIB_INFLATERAW:
        windowBits = CODEC_RAW_WINDOW_BITS;
        break;
    case ZLIB_UNZIP:
        windowBits = CODEC_UNZIP_WINDOW_BITS;
        break;
    }

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    GetConfigValue(opts, "level", level, true);
    GetConfigValue(opts, "memLevel", memLevel, true);
    GetConfigValue(opts, "strategy", strategy, true);

    // windowBits from opts overrides default only if explicitly set
    int32_t wb = 0;
    if (GetConfigValue(opts, "windowBits", wb, true) == 0)
        windowBits = wb;
}

// Gzip
result_t Gzip_base::_new(v8::Local<v8::Object> opts, obj_ptr<Gzip_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_GZIP, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<Gzip_base>(ZLIB_GZIP, level, windowBits, memLevel, strategy);
    return 0;
}

// Gunzip
result_t Gunzip_base::_new(v8::Local<v8::Object> opts, obj_ptr<Gunzip_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_GUNZIP, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<Gunzip_base>(ZLIB_GUNZIP, level, windowBits, memLevel, strategy);
    return 0;
}

// Deflate
result_t Deflate_base::_new(v8::Local<v8::Object> opts, obj_ptr<Deflate_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_DEFLATE, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<Deflate_base>(ZLIB_DEFLATE, level, windowBits, memLevel, strategy);
    return 0;
}

// Inflate
result_t Inflate_base::_new(v8::Local<v8::Object> opts, obj_ptr<Inflate_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_INFLATE, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<Inflate_base>(ZLIB_INFLATE, level, windowBits, memLevel, strategy);
    return 0;
}

// DeflateRaw
result_t DeflateRaw_base::_new(v8::Local<v8::Object> opts, obj_ptr<DeflateRaw_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_DEFLATERAW, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<DeflateRaw_base>(ZLIB_DEFLATERAW, level, windowBits, memLevel, strategy);
    return 0;
}

// InflateRaw
result_t InflateRaw_base::_new(v8::Local<v8::Object> opts, obj_ptr<InflateRaw_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_INFLATERAW, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<InflateRaw_base>(ZLIB_INFLATERAW, level, windowBits, memLevel, strategy);
    return 0;
}

// Unzip
result_t Unzip_base::_new(v8::Local<v8::Object> opts, obj_ptr<Unzip_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    extract_opts(opts, ZLIB_UNZIP, level, windowBits, memLevel, strategy);
    retVal = new ZlibCodecImpl<Unzip_base>(ZLIB_UNZIP, level, windowBits, memLevel, strategy);
    return 0;
}

} // namespace fibjs
