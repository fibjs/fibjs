/*
 * ZstdStream.h
 *
 *  Created on: Nov 12, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/zlib.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include <zstd/lib/zstd.h>
#include <zlib/include/zlib.h>
#include "AsyncStream.h"
#include "CodecUtils.h"

namespace fibjs {

// The output chunk handed to the destination stream. zstd emits at most one
// window per call, so a 128 KiB buffer makes the compress/decompress loop run a
// bounded number of times per input chunk.
#define ZSTD_CHUNK (128 * 1024)

// The flush flags of the fibjs stream wrappers are the zlib ones (write is
// Z_NO_FLUSH, flush is Z_SYNC_FLUSH, close is Z_FINISH); the zstd equivalents
// are its own end directives. The mapping keeps one set of flags for every
// codec in the module: a caller writing generic compression code does not have
// to know which backend a stream wraps.
inline ZSTD_EndDirective zstd_end_directive(int32_t flush)
{
    switch (flush) {
    case Z_FINISH:
        return ZSTD_e_end;
    case Z_SYNC_FLUSH:
    case Z_FULL_FLUSH:
    case Z_PARTIAL_FLUSH:
        return ZSTD_e_flush;
    default:
        return ZSTD_e_continue;
    }
}

// Node.js reports the enumerated zstd error as `code` and the free text as the
// message, so a caller can branch on the code while the log stays readable.
inline const char* zstd_error_code_name(ZSTD_ErrorCode code)
{
    switch (code) {
    case ZSTD_error_no_error: return "ZSTD_error_no_error";
    case ZSTD_error_GENERIC: return "ZSTD_error_GENERIC";
    case ZSTD_error_prefix_unknown: return "ZSTD_error_prefix_unknown";
    case ZSTD_error_version_unsupported: return "ZSTD_error_version_unsupported";
    case ZSTD_error_frameParameter_unsupported: return "ZSTD_error_frameParameter_unsupported";
    case ZSTD_error_frameParameter_windowTooLarge: return "ZSTD_error_frameParameter_windowTooLarge";
    case ZSTD_error_corruption_detected: return "ZSTD_error_corruption_detected";
    case ZSTD_error_checksum_wrong: return "ZSTD_error_checksum_wrong";
    case ZSTD_error_literals_headerWrong: return "ZSTD_error_literals_headerWrong";
    case ZSTD_error_dictionary_corrupted: return "ZSTD_error_dictionary_corrupted";
    case ZSTD_error_dictionary_wrong: return "ZSTD_error_dictionary_wrong";
    case ZSTD_error_dictionaryCreation_failed: return "ZSTD_error_dictionaryCreation_failed";
    case ZSTD_error_parameter_unsupported: return "ZSTD_error_parameter_unsupported";
    case ZSTD_error_parameter_combination_unsupported: return "ZSTD_error_parameter_combination_unsupported";
    case ZSTD_error_parameter_outOfBound: return "ZSTD_error_parameter_outOfBound";
    case ZSTD_error_tableLog_tooLarge: return "ZSTD_error_tableLog_tooLarge";
    case ZSTD_error_maxSymbolValue_tooLarge: return "ZSTD_error_maxSymbolValue_tooLarge";
    case ZSTD_error_maxSymbolValue_tooSmall: return "ZSTD_error_maxSymbolValue_tooSmall";
    case ZSTD_error_stabilityCondition_notRespected: return "ZSTD_error_stabilityCondition_notRespected";
    case ZSTD_error_stage_wrong: return "ZSTD_error_stage_wrong";
    case ZSTD_error_init_missing: return "ZSTD_error_init_missing";
    case ZSTD_error_memory_allocation: return "ZSTD_error_memory_allocation";
    case ZSTD_error_workSpace_tooSmall: return "ZSTD_error_workSpace_tooSmall";
    case ZSTD_error_dstSize_tooSmall: return "ZSTD_error_dstSize_tooSmall";
    case ZSTD_error_srcSize_wrong: return "ZSTD_error_srcSize_wrong";
    case ZSTD_error_dstBuffer_null: return "ZSTD_error_dstBuffer_null";
    case ZSTD_error_noForwardProgress_destFull: return "ZSTD_error_noForwardProgress_destFull";
    case ZSTD_error_noForwardProgress_inputEmpty: return "ZSTD_error_noForwardProgress_inputEmpty";
    default: return "ZSTD_error_GENERIC";
    }
}

// One error object for a failed zstd call: ZSTD_isError covers both the
// convenience functions and the streaming API, and the enumerated code plus
// ZSTD_getErrorName give the `code` / `message` pair the rest of the module
// reports.
inline result_t zstd_error(size_t code)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kError)
        .with_code(zstd_error_code_name(ZSTD_getErrorCode(code)))
        .with_message(ZSTD_getErrorName(code));
}

inline result_t zstd_no_context()
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kError)
        .with_code("ZSTD_error_memory_allocation")
        .with_message("zstd: could not allocate a context");
}

// The write side of the zstd wrappers: same shape as ZlibStream, but zstd has
// no z_stream, so the codec state and the in/out buffers live here instead.
class ZstdStream : public AsyncStream<Stream_base> {
private:
    class asyncWrite : public AsyncState {
    public:
        asyncWrite(ZstdStream* pThis, Stream_base* stm, Buffer_base* data, AsyncHandle ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(Z_NO_FLUSH)
        {
            m_data = Buffer::Cast(data);
            m_in.src = m_data->data();
            m_in.size = (size_t)m_data->length();
            m_in.pos = 0;

            m_pThis->m_done = false;
            init(process);
        }

        asyncWrite(ZstdStream* pThis, Stream_base* stm, int32_t flush, AsyncHandle ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_flush(flush)
        {
            m_in.src = NULL;
            m_in.size = 0;
            m_in.pos = 0;

            m_pThis->m_done = false;
            init(process);
        }

        ON_STATE(asyncWrite, process)
        {
            // The directive is already satisfied: this re-entry follows the
            // write of its last output block, so the operation is complete.
            // Without this check a finished compressor would start a new empty
            // frame on every round and never return.
            if (m_pThis->m_done && m_in.pos >= m_in.size)
                return next();

            m_out.dst = m_pThis->m_outBuffer;
            m_out.size = ZSTD_CHUNK;
            m_out.pos = 0;

            result_t hr = m_pThis->do_process(m_in, m_out, zstd_end_directive(m_flush));
            if (hr.failed())
                return hr;

            // Nothing produced: the codec absorbed the input (a compressor
            // buffers until it has a block, a decompressor may need more
            // bytes), so this write is done.
            if (m_out.pos == 0)
                return next();

            if (m_pThis->m_maxSize >= 0) {
                if ((int32_t)m_out.pos > m_pThis->m_maxSize - m_pThis->m_dataSize)
                    return max_output_exceeded(m_pThis->m_maxSize);
                m_pThis->m_dataSize += (int32_t)m_out.pos;
            }

            m_buffer = new Buffer((const char*)m_out.dst, (int32_t)m_out.pos);
            // Re-enter this state after the write: a flush or finish directive
            // is only satisfied once the codec stops producing output.
            return m_stm->writeBuffer(m_buffer, this);
        }

    private:
        obj_ptr<ZstdStream> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<Buffer_base> m_buffer;
        obj_ptr<Buffer> m_data;
        ZSTD_inBuffer m_in;
        ZSTD_outBuffer m_out;
        int32_t m_flush;
    };

public:
    ZstdStream(Stream_base* stm, int32_t maxSize = -1)
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
    // destination back.  It mirrors the zlib wrappers so `zstdCompress` and
    // `gzip` behave the same way from the caller's point of view.
    result_t process(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(ZstdStream* pThis, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
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
                // MemoryStream::readAll reports "no bytes" as null, but a
                // compression round trip returns an empty Buffer for empty
                // input, the way the zlib and brotli forms do.
                if (m_retVal == NULL)
                    m_retVal = new Buffer(NULL, 0);

                return next();
            }

        private:
            obj_ptr<ZstdStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
            obj_ptr<Buffer_base>& m_retVal;
            obj_ptr<MemoryStream> m_stm;
        };

        return (new asyncProcess(this, data, retVal, std::move(ac)))->post(result_t());
    }

    // The one-shot form when the caller writes into a destination directly:
    // the buffer is written, the frame is finished and the call returns.
    result_t process(Buffer_base* data, AsyncHandle ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(ZstdStream* pThis, Buffer_base* data, AsyncHandle ac)
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
            obj_ptr<ZstdStream> m_pThis;
            obj_ptr<Buffer_base> m_data;
        };

        return (new asyncProcess(this, data, std::move(ac)))->post(result_t());
    }

    result_t process(Stream_base* src, AsyncHandle ac)
    {
        class asyncProcess : public AsyncState {
        public:
            asyncProcess(ZstdStream* pThis, Stream_base* src, AsyncHandle ac)
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
            obj_ptr<ZstdStream> m_pThis;
            obj_ptr<Stream_base> m_src;
            int64_t m_size;
        };

        return (new asyncProcess(this, src, std::move(ac)))->post(result_t());
    }

public:
    virtual result_t do_process(ZSTD_inBuffer& in, ZSTD_outBuffer& out, ZSTD_EndDirective end) = 0;

protected:
    obj_ptr<Stream_base> m_stm;
    int32_t m_maxSize;
    int32_t m_dataSize;
    // Set by do_process when the current directive is satisfied (a finish or
    // flush that has nothing left to emit).
    bool m_done = false;
    unsigned char m_outBuffer[ZSTD_CHUNK];
};

class zdstream : public ZstdStream {
public:
    zdstream(Stream_base* stm, int32_t level, int32_t windowBits, int32_t nbWorkers,
        int64_t pledgedSrcSize = -1, int32_t strategy = -1)
        : ZstdStream(stm)
        , m_cctx(ZSTD_createCCtx())
        , m_level(level)
        , m_windowBits(windowBits)
        , m_nbWorkers(nbWorkers)
        , m_pledgedSrcSize(pledgedSrcSize)
        , m_strategy(strategy)
    {
        if (m_cctx)
            init_params();
    }

    ~zdstream()
    {
        if (m_cctx)
            ZSTD_freeCCtx(m_cctx);
    }

public:
    // Any other zstd parameter the caller passed through `options.params`.
    // The pairs are applied on top of the named knobs and are re-applied after
    // every reset, so a multi-frame stream keeps them.
    void set_params(const std::vector<std::pair<int32_t, int32_t>>& params)
    {
        m_params = params;
        if (m_cctx)
            init_params();
    }

public:
    virtual result_t do_process(ZSTD_inBuffer& in, ZSTD_outBuffer& out, ZSTD_EndDirective end)
    {
        if (!m_cctx)
            return zstd_no_context();

        size_t ret = ZSTD_compressStream2(m_cctx, &out, &in, end);
        if (ZSTD_isError(ret))
            return zstd_error(ret);

        // ret is the number of bytes still pending for the directive: 0 means
        // it is satisfied, so the write loop must stop even though the frame
        // produced output.
        m_done = (ret == 0);

        // A frame that ended leaves the context reusable for the next message,
        // the way deflateReset does after Z_FINISH; the parameters survive the
        // reset because only the session is cleared.
        if (end == ZSTD_e_end && ret == 0) {
            ZSTD_CCtx_reset(m_cctx, ZSTD_reset_session_only);
            init_params();
        }

        return result_t();
    }

private:
    void init_params()
    {
        ZSTD_CCtx_setParameter(m_cctx, ZSTD_c_compressionLevel, m_level);
        if (m_windowBits > 0)
            ZSTD_CCtx_setParameter(m_cctx, ZSTD_c_windowLog, m_windowBits);
        if (m_nbWorkers > 0)
            ZSTD_CCtx_setParameter(m_cctx, ZSTD_c_nbWorkers, m_nbWorkers);
        // zstd picks a strategy from the compression level; only an explicit
        // choice overrides it.
        if (m_strategy >= 0)
            ZSTD_CCtx_setParameter(m_cctx, ZSTD_c_strategy, m_strategy);

        // A pledged source size makes the frame header carry the content size,
        // which lets a decoder reserve the right amount of memory up front. The
        // session reset after Z_FINISH clears it, so it is set again for every
        // frame.
        if (m_pledgedSrcSize >= 0)
            ZSTD_CCtx_setPledgedSrcSize(m_cctx, (unsigned long long)m_pledgedSrcSize);

        for (const auto& p : m_params)
            ZSTD_CCtx_setParameter(m_cctx, (ZSTD_cParameter)p.first, p.second);
    }

private:
    ZSTD_CCtx* m_cctx;
    int32_t m_level;
    int32_t m_windowBits;
    int32_t m_nbWorkers;
    int64_t m_pledgedSrcSize;
    int32_t m_strategy;
    std::vector<std::pair<int32_t, int32_t>> m_params;
};

class zistream : public ZstdStream {
public:
    zistream(Stream_base* stm, int32_t maxSize, int32_t windowLogMax)
        : ZstdStream(stm, maxSize)
        , m_dctx(ZSTD_createDCtx())
        , m_windowLogMax(windowLogMax)
    {
        if (m_dctx && m_windowLogMax > 0)
            ZSTD_DCtx_setParameter(m_dctx, ZSTD_d_windowLogMax, m_windowLogMax);
    }

    ~zistream()
    {
        if (m_dctx)
            ZSTD_freeDCtx(m_dctx);
    }

public:
    // Any other zstd parameter the caller passed through `options.params`; a
    // decompressor has no compression knobs, only decoder limits.
    void set_params(const std::vector<std::pair<int32_t, int32_t>>& params)
    {
        if (!m_dctx)
            return;

        for (const auto& p : params)
            ZSTD_DCtx_setParameter(m_dctx, (ZSTD_dParameter)p.first, p.second);
    }

public:
    virtual result_t do_process(ZSTD_inBuffer& in, ZSTD_outBuffer& out, ZSTD_EndDirective end)
    {
        if (!m_dctx)
            return zstd_no_context();

        // decompressStream consumes as much input as it can and returns the
        // number of bytes still expected for the current frame: 0 means the
        // frame is complete and the next call starts a new one.
        size_t ret = ZSTD_decompressStream(m_dctx, &out, &in);
        if (ZSTD_isError(ret))
            return zstd_error(ret);

        m_done = (ret == 0 && in.pos == in.size);

        return result_t();
    }

private:
    ZSTD_DCtx* m_dctx;
    int32_t m_windowLogMax;
};

} /* namespace fibjs */
