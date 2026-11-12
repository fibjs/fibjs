/*
 * BrotliCodec.cpp
 *
 * The brotli codec classes (BrotliCompress / BrotliDecompress) and the parameter
 * reader the stream wrappers share with them.
 */

#include "object.h"
#include "ifs/BrotliCompress.h"
#include "ifs/BrotliDecompress.h"
#include "BrotliStream.h"
#include "Buffer.h"
#include "CodecUtils.h"
#include <brotli/decode.h>
#include <brotli/encode.h>
#include <zlib/include/zlib.h>

namespace fibjs {

// The output buffer of one compress/decompress round.
#define BROTLI_CODEC_CHUNK (128 * 1024)

// One error object for a failed decoder call: Node.js reports the decoder error
// name as `code`, and BrotliDecoderErrorString gives the message.
result_t brotli_decoder_error(BrotliDecoderErrorCode code)
{
    static const char* names[] = {
        "BROTLI_DECODER_NO_ERROR",
        "BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE",
        "BROTLI_DECODER_ERROR_FORMAT_RESERVED",
        "BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE",
        "BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET",
        "BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME",
        "BROTLI_DECODER_ERROR_FORMAT_CL_SPACE",
        "BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE",
        "BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT",
        "BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1",
        "BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2",
        "BROTLI_DECODER_ERROR_FORMAT_TRANSFORM",
        "BROTLI_DECODER_ERROR_FORMAT_DICTIONARY",
        "BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS",
        "BROTLI_DECODER_ERROR_FORMAT_PADDING_1",
        "BROTLI_DECODER_ERROR_FORMAT_PADDING_2",
        "BROTLI_DECODER_ERROR_FORMAT_DISTANCE",
        "BROTLI_DECODER_ERROR_DICTIONARY_NOT_SET",
        "BROTLI_DECODER_ERROR_INVALID_ARGUMENTS",
        "BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES",
        "BROTLI_DECODER_ERROR_ALLOC_TREE_GROUPS",
        "BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP",
        "BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1",
        "BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2",
        "BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES",
        "BROTLI_DECODER_ERROR_UNREACHABLE",
    };

    // The enum is negative and contiguous from -1; index by its magnitude.
    int32_t idx = -(int32_t)code;
    const char* name = (idx > 0 && idx < (int32_t)ARRAYSIZE(names))
        ? names[idx]
        : "BROTLI_DECODER_ERROR_UNREACHABLE";

    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kError)
        .with_code(name)
        .with_message("brotli: " + exlib::string(name));
}

// The `params` option: an object whose keys are the Brotli parameter ids.
// Reading it lives here because the codec classes and the stream wrappers take
// the same shape, and the two must agree on what a key means.
//
// This runs in the synchronous phase only: it walks v8 objects, and the values
// are copied into a plain vector so nothing v8-shaped survives the phase.
result_t brotli_options_from_options(v8::Local<v8::Object> opts, bool decoder,
    brotli_params& params)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // `level` is an alias of BROTLI_PARAM_QUALITY, the one compression knob the
    // deflate forms share with brotli. The decoder has no quality, so it drops
    // the alias instead of failing: Node.js accepts the same options object for
    // both directions.
    if (!decoder) {
        int32_t level = BROTLI_DEFAULT_QUALITY;
        result_t hr = GetConfigValue(opts, "level", level, true);
        if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
            return hr;
        if (hr.ok())
            params.push_back({ BROTLI_PARAM_QUALITY, level });
    }

    JSValue v = opts->Get(context, isolate->NewString("params"));
    if (v->IsUndefined() || v->IsNull())
        return result_t();

    if (!v->IsObject())
        return codec_bad_params();

    v8::Local<v8::Object> obj = v.As<v8::Object>();
    JSArray keys = obj->GetOwnPropertyNames(context);
    int32_t len = keys->Length();

    for (int32_t i = 0; i < len; i++) {
        JSValue k = keys->Get(context, i);
        exlib::string name = isolate->toString(k);

        // A key that is not a number is not a parameter id.
        if (name.empty() || name[0] < '0' || name[0] > '9')
            continue;

        int32_t param = 0;
        if (GetArgumentValue(isolate, k, param, false).failed())
            continue;

        v8::Local<v8::Value> raw;
        if (!obj->Get(context, k).ToLocal(&raw))
            continue;

        int32_t value = 0;
        result_t hr = GetArgumentValue(isolate, raw, value, true);
        if (hr.is_param_missing())
            continue;
        if (hr.failed())
            return hr;

        params.push_back({ param, value });
    }

    return result_t();
}

// The sync phase of an async call: read the options, then flatten them into the
// handle so the worker fiber can rebuild them without touching v8.
result_t brotli_options_to_ctx(v8::Local<v8::Object> options, bool decoder,
    AsyncHandle& ac)
{
    brotli_params params;
    result_t hr = brotli_options_from_options(options, decoder, params);
    if (hr.failed())
        return hr;

    ac.ctxv().resize(BROTLI_OPT_SLOTS + params.size() * 2);
    ac.ctxv()[0] = (int32_t)params.size();

    for (size_t i = 0; i < params.size(); i++) {
        ac.ctxv()[BROTLI_OPT_SLOTS + i * 2] = params[i].first;
        ac.ctxv()[BROTLI_OPT_SLOTS + i * 2 + 1] = params[i].second;
    }

    return result_t::nosync();
}

// The async phase counterpart of brotli_options_to_ctx.
result_t brotli_options_from_ctx(AsyncHandle& ac, brotli_params& params)
{
    if (ac.ctxv().size() < BROTLI_OPT_SLOTS)
        return result_t::make(CALL_E_INTERNAL)
            .with_type_name(errtype::kError)
            .with_code("ERR_INVALID_STATE")
            .with_message("zlib: the brotli options were not read");

    int32_t count = ac.ctxv()[0].intVal();
    for (int32_t i = 0; i < count; i++) {
        size_t at = BROTLI_OPT_SLOTS + i * 2;
        if (ac.ctxv().size() <= at + 1)
            break;
        params.push_back({ ac.ctxv()[at].intVal(), ac.ctxv()[at + 1].intVal() });
    }

    return result_t();
}

// ---------------------------------------------------------------------------
// Compressor codec
// ---------------------------------------------------------------------------

class BrotliCompressImpl : public BrotliCompress_base {
public:
    BrotliCompressImpl(v8::Local<v8::Object> opts)
        : m_state(BrotliEncoderCreateInstance(NULL, NULL, NULL))
        , m_closed(false)
    {
        if (!m_state) {
            m_closed = true;
            return;
        }

        std::vector<std::pair<int32_t, int32_t>> params;
        result_t hr = brotli_options_from_options(opts, false, params);
        if (hr.failed()) {
            m_closed = true;
            return;
        }

        m_params = params;
        for (const auto& p : params) {
            if (!BrotliEncoderSetParameter(m_state, (BrotliEncoderParameter)p.first, (uint32_t)p.second)) {
                m_closed = true;
                return;
            }
        }
    }

    ~BrotliCompressImpl()
    {
        if (m_state)
            BrotliEncoderDestroyInstance(m_state);
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
        const uint8_t* next_in = (const uint8_t*)input->data();
        size_t available_in = (size_t)input->length();

        BrotliEncoderOperation op = (flushFlag == Z_FINISH) ? BROTLI_OPERATION_FINISH
            : (flushFlag == Z_NO_FLUSH ? BROTLI_OPERATION_PROCESS : BROTLI_OPERATION_FLUSH);

        std::vector<uint8_t> output;

        while (true) {
            uint8_t outbuf[BROTLI_CODEC_CHUNK];
            size_t available_out = sizeof(outbuf);
            uint8_t* next_out = outbuf;

            size_t before = available_in;
            BrotliEncoderCompressStream(m_state, op, &available_in, &next_in,
                &available_out, &next_out, NULL);

            size_t produced = sizeof(outbuf) - available_out;
            if (produced > 0)
                output.insert(output.end(), outbuf, outbuf + produced);

            if (op == BROTLI_OPERATION_FINISH) {
                if (BrotliEncoderIsFinished(m_state)) {
                    // A finished encoder cannot be fed again, so the next
                    // chunk starts a new stream instead of failing or hanging.
                    // Node.js has no equivalent guarantee here (a second
                    // _processChunk after Z_FINISH is undefined behaviour), so
                    // this is the one place the two deliberately differ.
                    reinit();
                    break;
                }
            } else if (available_in == 0 && produced < sizeof(outbuf)) {
                // Nothing left to consume and the encoder did not fill the
                // buffer: the directive is satisfied.
                break;
            }

            // No progress at all would spin forever.
            if (before == available_in && produced == 0)
                break;
        }

        if (output.empty())
            retVal = new Buffer(NULL, 0);
        else
            retVal = new Buffer((const char*)output.data(), (int32_t)output.size());

        return result_t();
    }

    // Drops the finished encoder and builds a fresh one with the same
    // parameters, so the codec can serve another stream.
    void reinit()
    {
        BrotliEncoderDestroyInstance(m_state);
        m_state = BrotliEncoderCreateInstance(NULL, NULL, NULL);
        if (!m_state)
            return;

        for (const auto& p : m_params)
            BrotliEncoderSetParameter(m_state, (BrotliEncoderParameter)p.first,
                (uint32_t)p.second);
    }

    virtual result_t close() override
    {
        if (!m_closed && m_state) {
            BrotliEncoderDestroyInstance(m_state);
            m_state = NULL;
        }
        m_closed = true;
        return result_t();
    }

    virtual result_t reset() override
    {
        if (m_closed || !m_state)
            return result_t();

        reinit();
        if (!m_state)
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kError)
                .with_code("BROTLI_ENCODER_ERROR")
                .with_message("brotli: could not allocate an encoder");

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

    // Brotli parameters are fixed when the encoder is created; there is no
    // deflateParams equivalent, so this reports the call as unsupported.
    virtual result_t params(int32_t level, int32_t strategy) override
    {
        return result_t::invalid_call();
    }

private:
    BrotliEncoderState* m_state;
    bool m_closed;
    v8::Global<v8::Value> m_handle;
    std::vector<std::pair<int32_t, int32_t>> m_params;
};

// ---------------------------------------------------------------------------
// Decompressor codec
// ---------------------------------------------------------------------------

class BrotliDecompressImpl : public BrotliDecompress_base {
public:
    BrotliDecompressImpl(v8::Local<v8::Object> opts)
        : m_state(BrotliDecoderCreateInstance(NULL, NULL, NULL))
        , m_closed(false)
    {
        if (!m_state) {
            m_closed = true;
            return;
        }

        std::vector<std::pair<int32_t, int32_t>> params;
        result_t hr = brotli_options_from_options(opts, true, params);
        if (hr.failed()) {
            m_closed = true;
            return;
        }

        m_params = params;
        for (const auto& p : params)
            BrotliDecoderSetParameter(m_state, (BrotliDecoderParameter)p.first, (uint32_t)p.second);
    }

    ~BrotliDecompressImpl()
    {
        if (m_state)
            BrotliDecoderDestroyInstance(m_state);
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
        const uint8_t* next_in = (const uint8_t*)input->data();
        size_t available_in = (size_t)input->length();

        std::vector<uint8_t> output;

        while (true) {
            uint8_t outbuf[BROTLI_CODEC_CHUNK];
            size_t available_out = sizeof(outbuf);
            uint8_t* next_out = outbuf;

            BrotliDecoderResult result = BrotliDecoderDecompressStream(m_state,
                &available_in, &next_in, &available_out, &next_out, NULL);

            size_t produced = sizeof(outbuf) - available_out;
            if (produced > 0)
                output.insert(output.end(), outbuf, outbuf + produced);

            if (result == BROTLI_DECODER_RESULT_ERROR)
                return brotli_decoder_error(BrotliDecoderGetErrorCode(m_state));

            if (result == BROTLI_DECODER_RESULT_SUCCESS) {
                // The stream ended, so the decoder is spent; rebuild it so the
                // next chunk can decode another message.
                reinit();
                break;
            }

            // Needs more input and there is none, or needs more output and the
            // buffer was not filled: this call has done all it can.
            if (available_in == 0 && produced < sizeof(outbuf))
                break;
        }

        if (output.empty())
            retVal = new Buffer(NULL, 0);
        else
            retVal = new Buffer((const char*)output.data(), (int32_t)output.size());

        return result_t();
    }

    // Drops the spent decoder and builds a fresh one with the same options.
    void reinit()
    {
        BrotliDecoderDestroyInstance(m_state);
        m_state = BrotliDecoderCreateInstance(NULL, NULL, NULL);
        if (!m_state)
            return;

        for (const auto& p : m_params)
            BrotliDecoderSetParameter(m_state, (BrotliDecoderParameter)p.first,
                (uint32_t)p.second);
    }

    virtual result_t close() override
    {
        if (!m_closed && m_state) {
            BrotliDecoderDestroyInstance(m_state);
            m_state = NULL;
        }
        m_closed = true;
        return result_t();
    }

    virtual result_t reset() override
    {
        if (m_closed || !m_state)
            return result_t();

        reinit();
        if (!m_state)
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kError)
                .with_code("BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1")
                .with_message("brotli: could not allocate a decoder");

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
    BrotliDecoderState* m_state;
    bool m_closed;
    v8::Global<v8::Value> m_handle;
    std::vector<std::pair<int32_t, int32_t>> m_params;
};

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

result_t BrotliCompress_base::_new(v8::Local<v8::Object> opts, obj_ptr<BrotliCompress_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new BrotliCompressImpl(opts);
    return result_t();
}

result_t BrotliDecompress_base::_new(v8::Local<v8::Object> opts, obj_ptr<BrotliDecompress_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new BrotliDecompressImpl(opts);
    return result_t();
}

} // namespace fibjs
