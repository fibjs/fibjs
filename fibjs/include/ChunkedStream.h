/*
 * ChunkedStream.h
 *
 *  Created on: Jan 1, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/Stream.h"
#include "ifs/BufferedStream.h"
#include "ifs/io.h"
#include "Buffer.h"
#include "parse.h"
#include "AsyncStream.h"

namespace fibjs {

class ChunkedStream : public AsyncStream<Stream_base> {
public:
    ChunkedStream(BufferedStream_base* stm, int32_t maxChunkSize, int32_t maxBodySize)
        : m_stm(stm)
        , m_maxChunkSize(maxChunkSize)
        , m_maxBodySize(maxBodySize)
    {
    }

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        class asyncReadFrom : public AsyncState {
        public:
            asyncReadFrom(ChunkedStream* pThis, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_bytes(bytes)
                , m_retVal(retVal)
            {
                next(chunk_head);
            }

            ON_STATE(asyncReadFrom, chunk_head)
            {
                return m_pThis->m_stm->readLine(64, m_strLine, next(chunk_body));
            }

            ON_STATE(asyncReadFrom, chunk_body)
            {
                _parser p(m_strLine);
                char ch;
                int64_t sz = 0;

                p.skipSpace();

                if (!qisxdigit(p.get()))
                    return CHECK_ERROR(Runtime::setError("HttpMessage: bad chunk size."));

                while (qisxdigit(ch = p.get())) {
                    sz = (sz << 4) + qhex(ch);
                    p.skip();
                }

                if (sz < 0)
                    return CHECK_ERROR(Runtime::setError("HttpMessage: bad chunk size."));

                if (sz > 0) {
                    if (sz > (int64_t)m_pThis->m_maxChunkSize * 1024 * 1024)
                        return CHECK_ERROR(Runtime::setError("HttpMessage: chunk is too huge."));

                    m_pThis->m_contentLength += sz;
                    if (m_pThis->m_maxBodySize >= 0
                        && m_pThis->m_contentLength > (int64_t)m_pThis->m_maxBodySize * 1024 * 1024)
                        return CHECK_ERROR(Runtime::setError("HttpMessage: body is too huge."));

                    return m_pThis->m_stm->read(sz, m_pThis->m_data, next(chunk_body_end));
                }

                return m_pThis->m_stm->readLine(64, m_strLine, next(chunk_end));
            }

            ON_STATE(asyncReadFrom, chunk_body_end)
            {
                return m_pThis->m_stm->readLine(64, m_strLine, next(read));
            }

            ON_STATE(asyncReadFrom, read)
            {
                return m_pThis->read(m_bytes, m_retVal, next());
            }

            ON_STATE(asyncReadFrom, chunk_end)
            {
                return next(CALL_RETURN_NULL);
            }

        public:
            exlib::string m_strLine;
            obj_ptr<ChunkedStream> m_pThis;
            int32_t m_bytes;
            obj_ptr<Buffer_base>& m_retVal;
        };

        if (m_data) {
            Buffer* buf = m_data.As<Buffer>();

            if (bytes < 0 || bytes > buf->length() - m_pos)
                bytes = buf->length() - m_pos;

            retVal = bytes < buf->length() ? new Buffer(buf->data() + m_pos, bytes) : buf;
            m_pos += bytes;

            if (m_pos == buf->length()) {
                m_data.Release();
                m_pos = 0;
            }

            return 0;
        }

        return (new asyncReadFrom(this, bytes, retVal, ac))->post(0);
    }

    virtual result_t write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t flush(AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t close(AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
    {
        return io_base::copyStream(this, stm, bytes, retVal, ac);
    }

public:
    obj_ptr<BufferedStream_base> m_stm;
    int32_t m_maxChunkSize;
    int32_t m_maxBodySize;

    int64_t m_contentLength = 0;

    obj_ptr<Buffer_base> m_data;
    int32_t m_pos = 0;
};

}
