/*
 * WebSocketMessage.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "WebSocketMessage.h"
#include "Buffer.h"
#include "MemoryStream.h"

namespace fibjs {

result_t WebSocketMessage_base::_new(int32_t type, bool masked, int32_t maxSize,
    obj_ptr<WebSocketMessage_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new WebSocketMessage(type, masked, maxSize);
    return 0;
}

result_t WebSocketMessage::get_value(exlib::string& retVal)
{
    return m_message->get_value(retVal);
}

result_t WebSocketMessage::set_value(exlib::string newVal)
{
    return m_message->set_value(newVal);
}

result_t WebSocketMessage::get_params(obj_ptr<List_base>& retVal)
{
    return m_message->get_params(retVal);
}

result_t WebSocketMessage::set_params(List_base* newVal)
{
    return m_message->set_params(newVal);
}

result_t WebSocketMessage::get_body(obj_ptr<SeekableStream_base>& retVal)
{
    return m_message->get_body(retVal);
}

result_t WebSocketMessage::set_body(SeekableStream_base* newVal)
{
    return m_message->set_body(newVal);
}

result_t WebSocketMessage::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    return m_message->read(bytes, retVal, ac);
}

result_t WebSocketMessage::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return m_message->readAll(retVal, ac);
}

result_t WebSocketMessage::write(Buffer_base* data, AsyncEvent* ac)
{
    return m_message->write(data, ac);
}

result_t WebSocketMessage::get_length(int64_t& retVal)
{
    return m_message->get_length(retVal);
}

result_t WebSocketMessage::get_lastError(exlib::string& retVal)
{
    return m_message->get_lastError(retVal);
}

result_t WebSocketMessage::set_lastError(exlib::string newVal)
{
    return m_message->set_lastError(newVal);
}

result_t WebSocketMessage::end()
{
    return m_message->end();
}

result_t WebSocketMessage::isEnded(bool& retVal)
{
    return m_message->isEnded(retVal);
}

result_t WebSocketMessage::clear()
{
    m_message = new Message(m_bRep);
    return 0;
}

result_t WebSocketMessage::copy(Stream_base* from, Stream_base* to, int64_t bytes, uint32_t mask, AsyncEvent* ac)
{
    class asyncCopy : public AsyncState {
    public:
        asyncCopy(Stream_base* from, Stream_base* to, int64_t bytes, uint32_t mask, AsyncEvent* ac)
            : AsyncState(ac)
            , m_from(from)
            , m_to(to)
            , m_bytes(bytes)
            , m_mask(mask)
            , m_copyed(0)
        {
            set(read);
        }

        static int32_t read(AsyncState* pState, int32_t n)
        {
            asyncCopy* pThis = (asyncCopy*)pState;
            int64_t len;

            pThis->set(write);

            if (pThis->m_bytes == 0)
                return pThis->done();

            if (pThis->m_bytes > STREAM_BUFF_SIZE)
                len = STREAM_BUFF_SIZE;
            else
                len = pThis->m_bytes;

            pThis->m_buf.Release();
            return pThis->m_from->read((int32_t)len, pThis->m_buf, pThis);
        }

        static int32_t write(AsyncState* pState, int32_t n)
        {
            asyncCopy* pThis = (asyncCopy*)pState;
            int32_t blen;

            pThis->set(read);

            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));

            if (pThis->m_mask != 0) {
                exlib::string strBuffer;
                int32_t i, n;
                uint8_t* mask = (uint8_t*)&pThis->m_mask;

                pThis->m_buf->toString(strBuffer);

                n = (int32_t)strBuffer.length();
                for (i = 0; i < n; i++)
                    strBuffer[i] ^= mask[(pThis->m_copyed + i) & 3];

                pThis->m_buf = new Buffer(strBuffer);
            }

            pThis->m_buf->get_length(blen);
            pThis->m_copyed += blen;

            if (pThis->m_bytes > 0)
                pThis->m_bytes -= blen;

            return pThis->m_to->write(pThis->m_buf, pThis);
        }

    public:
        obj_ptr<Stream_base> m_from;
        obj_ptr<Stream_base> m_to;
        int64_t m_bytes;
        uint32_t m_mask;
        int64_t m_copyed;
        obj_ptr<Buffer_base> m_buf;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncCopy(from, to, bytes, mask, ac))->post(0);
}

result_t WebSocketMessage::sendTo(Stream_base* stm, AsyncEvent* ac)
{
    class asyncSendTo : public AsyncState {
    public:
        asyncSendTo(WebSocketMessage* pThis, Stream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_mask(0)
        {
            m_pThis->get_body(m_body);
            m_body->rewind();
            m_body->size(m_size);

            m_ms = new MemoryStream();

            set(head);
        }

        static int32_t head(AsyncState* pState, int32_t n)
        {
            asyncSendTo* pThis = (asyncSendTo*)pState;

            uint8_t buf[16];
            int32_t pos = 0;

            int32_t type;
            pThis->m_pThis->get_type(type);
            buf[0] = 0x80 | (type & 0x0f);

            int64_t size = pThis->m_size;
            if (size < 126) {
                buf[1] = (uint8_t)size;
                pos = 2;
            } else if (size < 65536) {
                buf[1] = 126;
                buf[2] = (uint8_t)(size >> 8);
                buf[3] = (uint8_t)(size & 0xff);
                pos = 4;
            } else {
                buf[1] = 127;
                buf[2] = (uint8_t)((size >> 56) & 0xff);
                buf[3] = (uint8_t)((size >> 48) & 0xff);
                buf[4] = (uint8_t)((size >> 40) & 0xff);
                buf[5] = (uint8_t)((size >> 32) & 0xff);
                buf[6] = (uint8_t)((size >> 24) & 0xff);
                buf[7] = (uint8_t)((size >> 16) & 0xff);
                buf[8] = (uint8_t)((size >> 8) & 0xff);
                buf[9] = (uint8_t)(size & 0xff);
                pos = 10;
            }

            if (pThis->m_pThis->m_masked) {
                buf[1] |= 0x80;

                uint32_t r = 0;
                while (r == 0)
                    r = rand();

                pThis->m_mask = r;

                buf[pos++] = (uint8_t)(r & 0xff);
                buf[pos++] = (uint8_t)((r >> 8) & 0xff);
                buf[pos++] = (uint8_t)((r >> 16) & 0xff);
                buf[pos++] = (uint8_t)((r >> 24) & 0xff);
            }

            pThis->m_buffer = new Buffer((const char*)buf, pos);

            pThis->set(sendData);
            return pThis->m_ms->write(pThis->m_buffer, pThis);
        }

        static int32_t sendData(AsyncState* pState, int32_t n)
        {
            asyncSendTo* pThis = (asyncSendTo*)pState;

            pThis->set(sendToStream);
            return copy(pThis->m_body, pThis->m_ms, pThis->m_size, pThis->m_mask, pThis);
        }

        static int32_t sendToStream(AsyncState* pState, int32_t n)
        {
            asyncSendTo* pThis = (asyncSendTo*)pState;

            pThis->m_ms->rewind();

            pThis->set(NULL);
            return io_base::copyStream(pThis->m_ms, pThis->m_stm, -1, pThis->m_size, pThis);
        }

    public:
        obj_ptr<MemoryStream_base> m_ms;
        WebSocketMessage* m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        int64_t m_size;
        uint32_t m_mask;
        obj_ptr<Buffer_base> m_buffer;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSendTo(this, stm, ac))->post(0);
}

result_t WebSocketMessage::readFrom(Stream_base* stm, AsyncEvent* ac)
{

    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(WebSocketMessage* pThis, Stream_base* stm,
            AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_fin(false)
            , m_masked(false)
            , m_fragmented(false)
            , m_size(0)
            , m_fullsize(0)
            , m_mask(0)
        {
            m_pThis->get_body(m_body);
            set(head);
        }

        static int32_t head(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            pThis->set(extHead);
            return pThis->m_stm->read(2, pThis->m_buffer, pThis);
        }

        static int32_t extHead(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            if (n == CALL_RETURN_NULL) {
                pThis->m_pThis->m_error = 1001;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
            }

            exlib::string strBuffer;
            char ch;
            int32_t sz = 0;

            pThis->m_buffer->toString(strBuffer);
            pThis->m_buffer.Release();

            ch = strBuffer[0];
            if (ch & 0x70) {
                pThis->m_pThis->m_error = 1007;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: non-zero RSV values."));
            }

            pThis->m_fin = (ch & 0x80) != 0;

            if (pThis->m_fragmented) {
                if ((ch & 0x0f) != ws_base::_CONTINUE) {
                    pThis->m_pThis->m_error = 1007;
                    return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
                }
            } else
                pThis->m_pThis->set_type(ch & 0x0f);

            ch = strBuffer[1];

            if (pThis->m_fragmented) {
                if (pThis->m_masked != (pThis->m_pThis->m_masked = (ch & 0x80) != 0)) {
                    pThis->m_pThis->m_error = 1007;
                    return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
                }
            } else
                pThis->m_masked = pThis->m_pThis->m_masked = (ch & 0x80) != 0;

            if (pThis->m_masked)
                sz += 4;

            pThis->m_size = ch & 0x7f;
            if (pThis->m_size == 126)
                sz += 2;
            else if (pThis->m_size == 127)
                sz += 8;

            if (sz) {
                pThis->set(extReady);
                return pThis->m_stm->read(sz, pThis->m_buffer, pThis);
            }

            pThis->set(body);
            return 0;
        }

        static int32_t extReady(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            if (n == CALL_RETURN_NULL) {
                pThis->m_pThis->m_error = 1007;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
            }

            exlib::string strBuffer;
            int32_t pos = 0;

            pThis->m_buffer->toString(strBuffer);
            pThis->m_buffer.Release();

            if (pThis->m_size == 126) {
                pThis->m_size = ((uint32_t)(uint8_t)strBuffer[0] << 8) + (uint8_t)strBuffer[1];
                pos += 2;
            } else if (pThis->m_size == 127) {
                pThis->m_size = ((int64_t)(uint8_t)strBuffer[0] << 56) + ((int64_t)(uint8_t)strBuffer[1] << 48) + ((int64_t)(uint8_t)strBuffer[2] << 40) + ((int64_t)(uint8_t)strBuffer[3] << 32) + ((int64_t)(uint8_t)strBuffer[4] << 24) + ((int64_t)(uint8_t)strBuffer[5] << 16) + ((int64_t)(uint8_t)strBuffer[6] << 8) + (int64_t)(uint8_t)strBuffer[7];
                pos += 8;
            }

            if (pThis->m_masked)
                memcpy(&pThis->m_mask, &strBuffer[pos], 4);

            pThis->set(body);
            return 0;
        }

        static int32_t body(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            if (pThis->m_fullsize + pThis->m_size > pThis->m_pThis->m_maxSize) {
                pThis->m_pThis->m_error = 1009;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: Message Too Big."));
            }

            pThis->set(body_end);
            return copy(pThis->m_stm, pThis->m_body, pThis->m_size, pThis->m_mask, pThis);
        }

        static int32_t body_end(AsyncState* pState, int32_t n)
        {
            asyncReadFrom* pThis = (asyncReadFrom*)pState;

            if (!pThis->m_fin) {
                pThis->m_fragmented = true;
                pThis->m_mask = 0;
                pThis->m_fullsize += pThis->m_size;
                pThis->set(head);
                return 0;
            }

            pThis->m_body->rewind();
            return pThis->done();
        }

    public:
        WebSocketMessage* m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Buffer_base> m_buffer;
        bool m_fin;
        bool m_masked;
        bool m_fragmented;
        int64_t m_size;
        int64_t m_fullsize;
        uint32_t m_mask;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_stm = stm;

    return (new asyncReadFrom(this, stm, ac))->post(0);
}

result_t WebSocketMessage::get_stream(obj_ptr<Stream_base>& retVal)
{
    if (!m_stm)
        return CALL_RETURN_NULL;

    retVal = m_stm;
    return 0;
}

result_t WebSocketMessage::get_response(obj_ptr<Message_base>& retVal)
{
    if (m_bRep)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!m_message->m_response) {
        int32_t type;
        get_type(type);

        if (type == ws_base::_PING)
            type = ws_base::_PONG;
        m_message->m_response = new WebSocketMessage(type, false, m_maxSize, true);
    }

    return m_message->get_response(retVal);
}

result_t WebSocketMessage::get_type(int32_t& retVal)
{
    return m_message->get_type(retVal);
}

result_t WebSocketMessage::set_type(int32_t newVal)
{
    return m_message->set_type(newVal);
}

result_t WebSocketMessage::get_data(v8::Local<v8::Value>& retVal)
{
    return m_message->get_data(retVal);
}

result_t WebSocketMessage::get_masked(bool& retVal)
{
    retVal = m_masked;
    return 0;
}

result_t WebSocketMessage::set_masked(bool newVal)
{
    m_masked = newVal;
    return 0;
}

result_t WebSocketMessage::get_maxSize(int32_t& retVal)
{
    retVal = m_maxSize;
    return 0;
}

result_t WebSocketMessage::set_maxSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxSize = newVal;
    return 0;
}

} /* namespace fibjs */
