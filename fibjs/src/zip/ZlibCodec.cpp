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
#include "CodecUtils.h"
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
    virtual result_t _processChunk(typename BASE::Union__processChunk_chunk chunk, int32_t flushFlag, obj_ptr<Buffer_base>& retVal) override
    {
        // the buffer form below does the work; the string alternative is
        // encoded as utf8 first
        if (std::holds_alternative<obj_ptr<Buffer_base>>(chunk))
            return _processChunk(std::get<obj_ptr<Buffer_base>>(chunk).get(), flushFlag, retVal);

        obj_ptr<Buffer_base> buf;
        result_t hr = Buffer_base::from(std::get<exlib::string>(chunk), "utf8", buf);
        if (hr.failed())
            return hr;

        return _processChunk(buf.get(), flushFlag, retVal);
    }

    virtual result_t _processChunk(Buffer_base* chunk, int32_t flushFlag, obj_ptr<Buffer_base>& retVal)
    {
        if (m_closed)
            return codec_closed();

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
                // Keep the message on the safe zlib-owned string. In the
                // stream helper path, strm.msg is not always a valid
                // null-terminated pointer once we surface the error.
                const char* msg = zError(ret);
                // Node.js exposes the zlib error constant as the code.
                const char* code = ret == Z_DATA_ERROR ? "Z_DATA_ERROR"
                    : (ret == Z_STREAM_ERROR ? "Z_STREAM_ERROR" : "Z_MEM_ERROR");

                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code(code).with_message(msg);
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

        return result_t();
    }

    virtual result_t close() override
    {
        cleanup();
        m_closed = true;
        return result_t();
    }

    virtual result_t reset() override
    {
        if (m_closed)
            return result_t();

        if (is_deflate())
            deflateReset(&m_strm);
        else
            inflateReset(&m_strm);

        return result_t();
    }

    virtual result_t get__handle(v8::Local<v8::Value>& retVal) override
    {
        return codec_get_handle(m_handle, this->holder(), retVal);
    }

    virtual result_t set__handle(v8::Local<v8::Value> newVal) override
    {
        return codec_set_handle(m_handle, this->holder(), newVal);
    }

    virtual result_t params(int32_t level, int32_t strategy) override
    {
        if (m_closed)
            return codec_closed();

        if (!is_deflate())
            return result_t::invalid_call();

        deflateParams(&m_strm, level, strategy);
        return result_t();
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

// Helper to extract options from the generated record
static result_t extract_opts(ZlibOptions* opts, ZlibMode mode,
    int32_t& level, int32_t& windowBits, int32_t& memLevel, int32_t& strategy)
{
    level = opts ? opts->level.value_or(Z_DEFAULT_COMPRESSION) : Z_DEFAULT_COMPRESSION;
    memLevel = opts ? opts->memLevel.value_or(8) : 8;
    strategy = opts ? opts->strategy.value_or(Z_DEFAULT_STRATEGY) : Z_DEFAULT_STRATEGY;

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

    // Node validates the level: a level outside the zlib range is
    // ERR_OUT_OF_RANGE rather than a clamped one, and it is validated on every
    // codec, including the inflating ones that never use it, so the same
    // options object is rejected the same way whichever class it is passed to.
    result_t hr = check_level(level);
    if (hr.failed())
        return hr;

    int32_t wb = opts ? opts->windowBits.value_or(0) : 0;
    if (wb != 0)
        windowBits = wb;

    return result_t();
}

// Gzip
result_t Gzip_base::_new(ZlibOptions* opts, obj_ptr<Gzip_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_GZIP, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<Gzip_base>(ZLIB_GZIP, level, windowBits, memLevel, strategy);
    return result_t();
}

// Gunzip
result_t Gunzip_base::_new(ZlibOptions* opts, obj_ptr<Gunzip_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_GUNZIP, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<Gunzip_base>(ZLIB_GUNZIP, level, windowBits, memLevel, strategy);
    return result_t();
}

// Deflate
result_t Deflate_base::_new(ZlibOptions* opts, obj_ptr<Deflate_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_DEFLATE, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<Deflate_base>(ZLIB_DEFLATE, level, windowBits, memLevel, strategy);
    return result_t();
}

// Inflate
result_t Inflate_base::_new(ZlibOptions* opts, obj_ptr<Inflate_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_INFLATE, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<Inflate_base>(ZLIB_INFLATE, level, windowBits, memLevel, strategy);
    return result_t();
}

// DeflateRaw
result_t DeflateRaw_base::_new(ZlibOptions* opts, obj_ptr<DeflateRaw_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_DEFLATERAW, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<DeflateRaw_base>(ZLIB_DEFLATERAW, level, windowBits, memLevel, strategy);
    return result_t();
}

// InflateRaw
result_t InflateRaw_base::_new(ZlibOptions* opts, obj_ptr<InflateRaw_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_INFLATERAW, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<InflateRaw_base>(ZLIB_INFLATERAW, level, windowBits, memLevel, strategy);
    return result_t();
}

// Unzip
result_t Unzip_base::_new(ZlibOptions* opts, obj_ptr<Unzip_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, memLevel, strategy;
    result_t ohr = extract_opts(opts, ZLIB_UNZIP, level, windowBits, memLevel, strategy);
    if (ohr.failed())
        return ohr;
    retVal = new ZlibCodecImpl<Unzip_base>(ZLIB_UNZIP, level, windowBits, memLevel, strategy);
    return result_t();
}

} // namespace fibjs
