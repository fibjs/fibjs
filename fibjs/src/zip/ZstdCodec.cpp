/*
 * ZstdCodec.cpp
 *
 * The zstd codec classes: ZstdCompress and ZstdDecompress. They implement the
 * same ZlibCodec interface as the zlib ones (a synchronous _processChunk driven
 * by the zlib flush flags), so code that drives a codec keeps working when the
 * backend changes.
 */

#include "object.h"
#include "ifs/ZstdCompress.h"
#include "ifs/ZstdDecompress.h"
#include "Buffer.h"
#include "CodecUtils.h"
#include "ZstdStream.h"

namespace fibjs {

// The `params` option: an object whose keys are zstd parameter constants and
// whose values are handed to ZSTD_CCtx_setParameter / ZSTD_DCtx_setParameter.
// It is the only way to reach the dozens of tuning knobs the library exposes
// without mirroring every one of them as a named option.
template <typename CtxT, typename SetT>
static result_t apply_params(CtxT* ctx, v8::Local<v8::Object> params, SetT set)
{
    if (params.IsEmpty())
        return result_t();

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    JSArray keys = params->GetOwnPropertyNames(context);
    int32_t len = keys->Length();
    int32_t i;

    for (i = 0; i < len; i++) {
        JSValue k = keys->Get(context, i);
        exlib::string name = isolate->toString(k);

        // A key that is not a number (the option object may carry the named
        // options of the codec, or anything else the caller added) is ignored.
        if (name.empty() || name[0] < '0' || name[0] > '9')
            continue;

        int32_t param = 0;
        if (GetArgumentValue(isolate, k, param, false).failed())
            continue;

        JSValue v = params->Get(context, k);
        int32_t value = 0;
        result_t hr = GetArgumentValue(isolate, v, value, true);
        if (hr.is_param_missing())
            continue;
        if (hr.failed())
            return hr;

        size_t ret = set(ctx, param, value);
        if (ZSTD_isError(ret))
            return zstd_error(ret);
    }

    return result_t();
}

// ---------------------------------------------------------------------------
// Compressor
// ---------------------------------------------------------------------------

class ZstdCompressImpl : public ZstdCompress_base {
public:
    ZstdCompressImpl(int32_t level, int32_t windowBits, int32_t strategy,
        int32_t nbWorkers, int64_t pledgedSrcSize, v8::Local<v8::Object> params)
        : m_cctx(ZSTD_createCCtx())
        , m_closed(false)
    {
        if (!m_cctx) {
            m_closed = true;
            return;
        }

        m_level = level;
        m_windowBits = windowBits;
        m_strategy = strategy;
        m_nbWorkers = nbWorkers;
        m_pledgedSrcSize = pledgedSrcSize;

        result_t hr = init_params(params);
        if (hr.failed())
            m_closed = true;
    }

    ~ZstdCompressImpl()
    {
        if (m_cctx)
            ZSTD_freeCCtx(m_cctx);
    }

public:
    // ZlibCodec_base
    virtual result_t _processChunk(ZlibCodec_base::Union__processChunk_chunk chunk, int32_t flushFlag, obj_ptr<Buffer_base>& retVal) override
    {
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

        ZSTD_inBuffer in;
        in.src = input->length() > 0 ? input->data() : (const void*)"";
        in.size = (size_t)input->length();
        in.pos = 0;

        ZSTD_EndDirective end = zstd_end_directive(flushFlag);
        std::vector<uint8_t> output;

        // ZSTD_compressStream2 keeps producing output until it returns 0 for
        // the current directive, so the loop stops when it reports completion
        // rather than when the input ran out: a flush directive is satisfied
        // only after the pending data has been written.
        while (true) {
            uint8_t outbuf[ZSTD_CHUNK];
            ZSTD_outBuffer out;
            out.dst = outbuf;
            out.size = sizeof(outbuf);
            out.pos = 0;

            size_t ret = ZSTD_compressStream2(m_cctx, &out, &in, end);
            if (ZSTD_isError(ret))
                return zstd_error(ret);

            if (out.pos > 0)
                output.insert(output.end(), outbuf, outbuf + out.pos);

            // ret is the number of bytes still pending for the directive: 0
            // means the directive is satisfied, which is the only condition
            // that ends the loop. A full output buffer always needs another
            // round, whatever ret says.
            if (ret == 0 && out.pos < out.size)
                break;

            if (end == ZSTD_e_continue && in.pos == in.size && out.pos < out.size)
                break;
        }

        if (end == ZSTD_e_end) {
            // A finished frame leaves the context ready for the next message,
            // the way deflateReset does after Z_FINISH.
            ZSTD_CCtx_reset(m_cctx, ZSTD_reset_session_only);
        }

        if (output.empty())
            retVal = new Buffer(NULL, 0);
        else
            retVal = new Buffer((const char*)output.data(), (int32_t)output.size());

        return result_t();
    }

    virtual result_t close() override
    {
        if (!m_closed && m_cctx) {
            ZSTD_freeCCtx(m_cctx);
            m_cctx = NULL;
        }
        m_closed = true;
        return result_t();
    }

    virtual result_t reset() override
    {
        if (m_closed || !m_cctx)
            return result_t();

        ZSTD_CCtx_reset(m_cctx, ZSTD_reset_session_and_parameters);
        init_params(v8::Local<v8::Object>());
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

        m_level = level;
        m_strategy = strategy;

        result_t hr = set_parameter(ZSTD_c_compressionLevel, level);
        if (hr.failed())
            return hr;

        return set_parameter(ZSTD_c_strategy, strategy);
    }

private:
    result_t init_params(v8::Local<v8::Object> params)
    {
        result_t hr = set_parameter(ZSTD_c_compressionLevel, m_level);
        if (hr.failed())
            return hr;

        // Only a strategy the caller asked for is applied: zstd chooses one
        // from the compression level, and overriding it here would make the
        // high levels compress worse than the low ones.
        if (m_strategy >= 0) {
            hr = set_parameter(ZSTD_c_strategy, m_strategy);
            if (hr.failed())
                return hr;
        }

        if (m_windowBits > 0) {
            hr = set_parameter(ZSTD_c_windowLog, m_windowBits);
            if (hr.failed())
                return hr;
        }

        if (m_nbWorkers > 0) {
            hr = set_parameter(ZSTD_c_nbWorkers, m_nbWorkers);
            if (hr.failed())
                return hr;
        }

        // A pledged source size writes the content size into the frame header,
        // which lets the decoder reserve the right amount of memory up front.
        if (m_pledgedSrcSize >= 0) {
            hr = set_parameter(ZSTD_c_contentSizeFlag, 1);
            if (hr.failed())
                return hr;

            size_t ret = ZSTD_CCtx_setPledgedSrcSize(m_cctx, (unsigned long long)m_pledgedSrcSize);
            if (ZSTD_isError(ret))
                return zstd_error(ret);
        }

        return apply_params(m_cctx, params,
            [](ZSTD_CCtx* ctx, int param, int value) {
                return ZSTD_CCtx_setParameter(ctx, (ZSTD_cParameter)param, value);
            });
    }

    result_t set_parameter(ZSTD_cParameter param, int32_t value)
    {
        size_t ret = ZSTD_CCtx_setParameter(m_cctx, param, value);
        if (ZSTD_isError(ret))
            return zstd_error(ret);

        return result_t();
    }

private:
    ZSTD_CCtx* m_cctx;
    bool m_closed;
    int32_t m_level = ZSTD_CLEVEL_DEFAULT;
    int32_t m_windowBits = 0;
    int32_t m_strategy = -1;
    int32_t m_nbWorkers = 0;
    int64_t m_pledgedSrcSize = -1;
    v8::Global<v8::Value> m_handle;
};

// ---------------------------------------------------------------------------
// Decompressor
// ---------------------------------------------------------------------------

class ZstdDecompressImpl : public ZstdDecompress_base {
public:
    ZstdDecompressImpl(int32_t windowLogMax, v8::Local<v8::Object> params)
        : m_dctx(ZSTD_createDCtx())
        , m_closed(false)
    {
        if (!m_dctx) {
            m_closed = true;
            return;
        }

        if (windowLogMax > 0) {
            result_t hr = set_parameter(ZSTD_d_windowLogMax, windowLogMax);
            if (hr.failed())
                m_closed = true;
            else
                m_windowLogMax = windowLogMax;
        }

        if (!m_closed) {
            result_t hr = apply_params(m_dctx, params,
                [](ZSTD_DCtx* ctx, int param, int value) {
                    return ZSTD_DCtx_setParameter(ctx, (ZSTD_dParameter)param, value);
                });
            if (hr.failed())
                m_closed = true;
        }
    }

    ~ZstdDecompressImpl()
    {
        if (m_dctx)
            ZSTD_freeDCtx(m_dctx);
    }

public:
    // ZlibCodec_base
    virtual result_t _processChunk(ZlibCodec_base::Union__processChunk_chunk chunk, int32_t flushFlag, obj_ptr<Buffer_base>& retVal) override
    {
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

        ZSTD_inBuffer in;
        in.src = input->length() > 0 ? input->data() : (const void*)"";
        in.size = (size_t)input->length();
        in.pos = 0;

        std::vector<uint8_t> output;

        // decompressStream returns the number of bytes still expected for the
        // current frame: 0 means the frame ended and the next call starts a new
        // one, which is how concatenated frames are decoded in one chain.
        while (true) {
            uint8_t outbuf[ZSTD_CHUNK];
            ZSTD_outBuffer out;
            out.dst = outbuf;
            out.size = sizeof(outbuf);
            out.pos = 0;

            size_t ret = ZSTD_decompressStream(m_dctx, &out, &in);
            if (ZSTD_isError(ret))
                return zstd_error(ret);

            if (out.pos > 0)
                output.insert(output.end(), outbuf, outbuf + out.pos);

            if (in.pos == in.size && out.pos < out.size)
                break;
        }

        if (output.empty())
            retVal = new Buffer(NULL, 0);
        else
            retVal = new Buffer((const char*)output.data(), (int32_t)output.size());

        return result_t();
    }

    virtual result_t close() override
    {
        if (!m_closed && m_dctx) {
            ZSTD_freeDCtx(m_dctx);
            m_dctx = NULL;
        }
        m_closed = true;
        return result_t();
    }

    virtual result_t reset() override
    {
        if (m_closed || !m_dctx)
            return result_t();

        ZSTD_DCtx_reset(m_dctx, ZSTD_reset_session_only);
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

    // A decompressor has no compression parameters to change.
    virtual result_t params(int32_t level, int32_t strategy) override
    {
        return result_t::invalid_call();
    }

private:
    result_t set_parameter(ZSTD_dParameter param, int32_t value)
    {
        size_t ret = ZSTD_DCtx_setParameter(m_dctx, param, value);
        if (ZSTD_isError(ret))
            return zstd_error(ret);

        return result_t();
    }

private:
    ZSTD_DCtx* m_dctx;
    bool m_closed;
    int32_t m_windowLogMax = 0;
    v8::Global<v8::Value> m_handle;
};

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

// Reads the shared option object. `level`, `windowBits`, `strategy` and
// `nbWorkers` are the named knobs; `params` carries any other zstd parameter by
// its numeric constant. A type mismatch is reported the way the zlib codecs do,
// instead of silently falling back to the default.
static result_t extract_opts(v8::Local<v8::Object> opts, int32_t defaultLevel,
    int32_t& level, int32_t& windowBits, int32_t& strategy, int32_t& nbWorkers,
    int64_t& pledgedSrcSize, v8::Local<v8::Object>& params)
{
    level = defaultLevel;
    windowBits = 0;
    strategy = -1;
    nbWorkers = 0;
    pledgedSrcSize = -1;

    result_t hr = GetConfigValue(opts, "level", level, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(opts, "windowBits", windowBits, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(opts, "strategy", strategy, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(opts, "nbWorkers", nbWorkers, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(opts, "pledgedSrcSize", pledgedSrcSize, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    JSValue v = opts->Get(context, isolate->NewString("params"));
    if (!v->IsUndefined() && !v->IsNull()) {
        if (!v->IsObject())
            return codec_bad_params();

        params = v.As<v8::Object>();
    }

    return result_t();
}

result_t ZstdCompress_base::_new(v8::Local<v8::Object> opts, obj_ptr<ZstdCompress_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, strategy, nbWorkers;
    int64_t pledgedSrcSize;
    v8::Local<v8::Object> params;

    result_t hr = extract_opts(opts, ZSTD_CLEVEL_DEFAULT, level, windowBits, strategy,
        nbWorkers, pledgedSrcSize, params);
    if (hr.failed())
        return hr;

    retVal = new ZstdCompressImpl(level, windowBits, strategy, nbWorkers,
        pledgedSrcSize, params);
    return result_t();
}

result_t ZstdDecompress_base::_new(v8::Local<v8::Object> opts, obj_ptr<ZstdDecompress_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t level, windowBits, strategy, nbWorkers;
    int64_t pledgedSrcSize;
    v8::Local<v8::Object> params;

    result_t hr = extract_opts(opts, ZSTD_CLEVEL_DEFAULT, level, windowBits, strategy,
        nbWorkers, pledgedSrcSize, params);
    if (hr.failed())
        return hr;

    retVal = new ZstdDecompressImpl(windowBits, params);
    return result_t();
}

} // namespace fibjs
