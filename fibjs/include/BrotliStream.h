/*
 * BrotliStream.h
 *
 *  Created on: Nov 12, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/zlib.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <brotli/decode.h>
#include <brotli/encode.h>
#include <zlib/include/zlib.h>
#include "AsyncStream.h"

namespace fibjs {

// The output chunk handed to the destination stream. Brotli emits at most one
// meta-block per round, so this bounds the number of compress calls per write
// without holding a whole window on the stack.
#define BROTLI_CHUNK (128 * 1024)

// The stream wrappers of this module take the zlib flush flags (write is
// Z_NO_FLUSH, flush is Z_SYNC_FLUSH, close is Z_FINISH) so that one piece of
// compression code works with every backend. Brotli has its own operations, so
// they are derived here instead of leaking a second set of flags into the API.
inline BrotliEncoderOperation brotli_operation(int32_t flush)
{
    switch (flush) {
    case Z_FINISH:
        return BROTLI_OPERATION_FINISH;
    case Z_SYNC_FLUSH:
    case Z_FULL_FLUSH:
    case Z_PARTIAL_FLUSH:
        return BROTLI_OPERATION_FLUSH;
    default:
        return BROTLI_OPERATION_PROCESS;
    }
}

// One error object for a failed brotli call. Node.js reports the decoder result
// as `code` (`BROTLI_DECODER_ERROR_*`); an encoder that runs out of memory has
// no such enum, so it reports the generic code instead.
inline result_t brotli_error(const char* code, const char* message)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kError)
        .with_code(code)
        .with_message(message);
}

// Node.js stops a decompression that would grow past `maxOutputLength` with
// ERR_BUFFER_TOO_LARGE (a RangeError); the error number stays fibjs's own 20013.
inline result_t brotli_max_output_exceeded(int32_t maxSize)
{
    return result_t::make(CALL_E_OVERFLOW)
        .with_type_name(errtype::kRangeError)
        .with_code("ERR_BUFFER_TOO_LARGE")
        .with_format("Cannot create a Buffer larger than %d bytes", maxSize);
}

// A failed decoder carries the enumeration name as `code`. Both the stream
// wrappers and the codec classes report it, so the mapping lives in one place.
result_t brotli_decoder_error(BrotliDecoderErrorCode code);

// Brotli's options are read once, in the synchronous phase of a call, and
// flattened into AsyncHandle slots, because the async phase runs on a worker
// fiber and may not touch v8 handles. Slot 0 holds the parameter count and each
// parameter then takes two slots (id, value).
#define BROTLI_OPT_SLOTS 1

typedef std::vector<std::pair<int32_t, int32_t>> brotli_params;

// The single reader of an options object; `decoder` selects the two parameters
// the decoder understands and drops the encoder-only `level` alias.
result_t brotli_options_from_options(v8::Local<v8::Object> options, bool decoder,
    brotli_params& params);

// Called in the sync phase: read the options and store them in the handle.
result_t brotli_options_to_ctx(v8::Local<v8::Object> options, bool decoder,
    AsyncHandle& ac);

// Called in the async phase: rebuild the parameters from the slots.
result_t brotli_options_from_ctx(AsyncHandle& ac, brotli_params& params);

// The write side of the brotli wrappers: the same shape as ZlibStream, with the
// encoder/decoder state kept in an opaque context and the buffers as plain
// pointers instead of a z_stream.
class BrotliStream : public AsyncStream<Stream_base> {
private:
    class asyncWrite : public AsyncState {
    public:
        asyncWrite(BrotliStream* pThis, Stream_base* stm, Buffer_base* data, AsyncHandle ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(Z_NO_FLUSH)
        {
            m_data = Buffer::Cast(data);
            m_in = m_data->data();
            m_inSize = (size_t)m_data->length();

            init(process);
        }

        asyncWrite(BrotliStream* pThis, Stream_base* stm, int32_t flush, AsyncHandle ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(flush)
        {
            m_in = NULL;
            m_inSize = 0;

            init(process);
        }

        ON_STATE(asyncWrite, process)
        {
            // Brotli advances the output pointer and decrements the count
            // itself, so the bytes produced this round are what the chunk lost.
            size_t outAvailable = BROTLI_CHUNK;
            m_out = m_pThis->m_outBuffer;

            result_t hr = m_pThis->do_process(&m_in, &m_inSize, &m_out,
                &outAvailable, m_flush);
            if (hr.failed())
                return hr;

            m_outPos = BROTLI_CHUNK - outAvailable;

            // Nothing produced: the codec absorbed the input (an encoder
            // buffers until it has a meta-block, a decoder may need more
            // bytes), so this write is done.
            if (m_outPos == 0)
                return next();

            if (m_pThis->m_maxSize >= 0) {
                if ((int32_t)m_outPos > m_pThis->m_maxSize - m_pThis->m_dataSize)
                    return brotli_max_output_exceeded(m_pThis->m_maxSize);
                m_pThis->m_dataSize += (int32_t)m_outPos;
            }

            // The codec advanced `m_out` past the bytes it wrote, so the chunk
            // itself starts at the buffer's base, not at the cursor.
            m_buffer = new Buffer((const char*)m_pThis->m_outBuffer, (int32_t)m_outPos);
            // Re-enter this state after the write: a flush or finish directive
            // is only satisfied once the codec stops producing output.
            return m_stm->writeBuffer(m_buffer, this);
        }

    private:
        obj_ptr<BrotliStream> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<Buffer_base> m_buffer;
        obj_ptr<Buffer> m_data;
        const uint8_t* m_in;
        size_t m_inSize;
        uint8_t* m_out;
        size_t m_outSize;
        size_t m_outPos;
        int32_t m_flush;
    };

public:
    BrotliStream(Stream_base* stm, int32_t maxSize = -1)
        : m_stm(stm)
        , m_maxSize(maxSize)
        , m_dataSize(0)
    {
    }

public:
    // Stream_base
    result_t get_fd(int32_t& retVal)
    {
        return result_t::invalid_call();
    }

    result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
    {
        return result_t::invalid_call();
    }

    result_t writeBuffer(Buffer_base* data, AsyncHandle ac)
    {
        return (new asyncWrite(this, m_stm, data, std::move(ac)))->post(result_t());
    }

    result_t flush(AsyncHandle ac)
    {
        return (new asyncWrite(this, m_stm, Z_SYNC_FLUSH, std::move(ac)))->post(result_t());
    }

    result_t close(AsyncHandle ac)
    {
        return (new asyncWrite(this, m_stm, Z_FINISH, std::move(ac)))->post(result_t());
    }

public:
    // The one-shot form: write everything, finish the stream, then read the
    // destination back, so `brotliCompress` and `gzip` behave the same way from
    // the caller's point of view.
    result_t process(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(BrotliStream* pThis, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_data(data)
                , m_retVal(retVal)
            {
                m_stm = new MemoryStream();
                m_pThis->m_stm = m_stm;

                init(write);
            }

            ON_STATE(asyncProcess, write)
            {
                return m_pThis->writeBuffer(m_data, next(write_ok));
            }

            ON_STATE(asyncProcess, write_ok)
            {
                return m_pThis->close(next(read));
            }

            ON_STATE(asyncProcess, read)
            {
                m_stm->rewind();
                return m_stm->readAll(m_retVal, next(read_ok));
            }

            ON_STATE(asyncProcess, read_ok)
            {
                // MemoryStream::readAll reports "no bytes" as null, which suits
                // a plain stream but not a compression round trip: Node.js
                // returns an empty Buffer for empty input, and a caller writing
                // `result.length` would throw on null.
                if (m_retVal == NULL)
                    m_retVal = new Buffer(NULL, 0);

                return next();
            }

        private:
            obj_ptr<BrotliStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
            obj_ptr<Buffer_base>& m_retVal;
            obj_ptr<MemoryStream> m_stm;
        };

        return (new asyncProcess(this, data, retVal, std::move(ac)))->post(result_t());
    }

    result_t process(Buffer_base* data, AsyncHandle ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(BrotliStream* pThis, Buffer_base* data, AsyncHandle ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_data(data)
            {
                init(write);
            }

            ON_STATE(asyncProcess, write)
            {
                return m_pThis->writeBuffer(m_data, next(write_ok));
            }

            ON_STATE(asyncProcess, write_ok)
            {
                return m_pThis->close(next());
            }

        private:
            obj_ptr<BrotliStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
        };

        return (new asyncProcess(this, data, std::move(ac)))->post(result_t());
    }

    result_t process(Stream_base* src, AsyncHandle ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(BrotliStream* pThis, Stream_base* src, AsyncHandle ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_src(src)
            {
                init(copy);
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
            obj_ptr<BrotliStream> m_pThis;
            obj_ptr<Stream_base> m_src;
            int64_t m_size;
        };

        return (new asyncProcess(this, src, std::move(ac)))->post(result_t());
    }

public:
    // Runs one round of the codec. `next_in`/`available_in` describe the
    // input, `next_out`/`available_out` the output chunk; the codec advances
    // both pointers, so the produced size is what `available_out` lost.
    // `flush` carries the zlib flag of the operation being served.
    virtual result_t do_process(const uint8_t** next_in, size_t* available_in,
        uint8_t** next_out, size_t* available_out, int32_t flush) = 0;

protected:
    obj_ptr<Stream_base> m_stm;
    int32_t m_maxSize;
    int32_t m_dataSize;
    unsigned char m_outBuffer[BROTLI_CHUNK];
};

// The compressor side of the stream wrappers (`createBrotliCompress`).
class brstream : public BrotliStream {
public:
    brstream(Stream_base* stm, const brotli_params& params)
        : BrotliStream(stm)
        , m_state(BrotliEncoderCreateInstance(NULL, NULL, NULL))
    {
        if (!m_state)
            return;

        for (const auto& p : params)
            if (!BrotliEncoderSetParameter(m_state, (BrotliEncoderParameter)p.first,
                    (uint32_t)p.second)) {
                // A rejected value leaves the encoder unusable; report it from
                // do_process instead of silently compressing with defaults.
                BrotliEncoderDestroyInstance(m_state);
                m_state = NULL;
                return;
            }
    }

    ~brstream()
    {
        if (m_state)
            BrotliEncoderDestroyInstance(m_state);
    }

public:
    virtual result_t do_process(const uint8_t** next_in, size_t* available_in,
        uint8_t** next_out, size_t* available_out, int32_t flush)
    {
        if (!m_state)
            return brotli_error("BROTLI_ENCODER_ERROR", "brotli: could not allocate an encoder");

        size_t before_in = *available_in;
        size_t before_out = *available_out;

        // Returns BROTLI_BOOL, not a byte count: success only says the call
        // did not fail, the progress is read from the two counters.
        if (!BrotliEncoderCompressStream(m_state, brotli_operation(flush),
                available_in, next_in, available_out, next_out, NULL))
            return brotli_error("BROTLI_ENCODER_ERROR", "brotli: encoder failed");

        // A process round legitimately absorbs input without emitting
        // anything (the encoder buffers until it has a meta-block), but a flush
        // or finish round is only satisfied once output appears, so a round
        // that moved neither counter would spin the write loop forever.
        if (flush != Z_NO_FLUSH && *available_in == before_in
            && *available_out == before_out
            && !BrotliEncoderIsFinished(m_state)
            && !BrotliEncoderHasMoreOutput(m_state))
            return brotli_error("BROTLI_ENCODER_ERROR", "brotli: encoder made no progress");

        return result_t();
    }

private:
    BrotliEncoderState* m_state;
};

// The decompressor side of the stream wrappers (`createBrotliDecompress`).
class bdstream : public BrotliStream {
public:
    bdstream(Stream_base* stm, int32_t maxSize, const brotli_params& params)
        : BrotliStream(stm, maxSize)
        , m_state(BrotliDecoderCreateInstance(NULL, NULL, NULL))
    {
        if (!m_state)
            return;

        // The decoder only has options that relax parsing, and a rejected one
        // is fatal for the same reason as on the encoder side.
        for (const auto& p : params)
            if (!BrotliDecoderSetParameter(m_state, (BrotliDecoderParameter)p.first,
                    (uint32_t)p.second)) {
                BrotliDecoderDestroyInstance(m_state);
                m_state = NULL;
                return;
            }
    }

    ~bdstream()
    {
        if (m_state)
            BrotliDecoderDestroyInstance(m_state);
    }

public:
    virtual result_t do_process(const uint8_t** next_in, size_t* available_in,
        uint8_t** next_out, size_t* available_out, int32_t flush)
    {
        if (!m_state)
            return brotli_error("BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1", "brotli: could not allocate a decoder");

        BrotliDecoderResult result = BrotliDecoderDecompressStream(m_state,
            available_in, next_in, available_out, next_out, NULL);

        // Finishing a decoder that never reached the end of the stream means
        // the input was truncated. Node.js reports that as Z_BUF_ERROR with
        // "unexpected end of file" instead of handing back the partial output,
        // so a caller cannot mistake a cut-off message for a complete one.
        if (flush == Z_FINISH && result != BROTLI_DECODER_RESULT_SUCCESS
            && result != BROTLI_DECODER_RESULT_ERROR)
            return brotli_error("Z_BUF_ERROR", "unexpected end of file");

        if (result == BROTLI_DECODER_RESULT_ERROR)
            return brotli_decoder_error(BrotliDecoderGetErrorCode(m_state));

        return result_t();
    }

private:
    BrotliDecoderState* m_state;
};

} /* namespace fibjs */
