/*
 * WebSocketMessage.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "ifs/zlib.h"
#include "WebSocketMessage.h"
#include "Buffer.h"
#include "MemoryStream.h"

namespace fibjs {

result_t WebSocketMessage_base::_new(int32_t type, bool masked, bool compress, int32_t maxSize,
    obj_ptr<WebSocketMessage_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new WebSocketMessage(type, masked, compress, maxSize);
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

result_t WebSocketMessage::get_params(obj_ptr<NArray>& retVal)
{
    return m_message->get_params(retVal);
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

result_t WebSocketMessage::json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    return m_message->json(data, retVal);
}

result_t WebSocketMessage::json(v8::Local<v8::Value>& retVal)
{
    return m_message->json(retVal);
}

result_t WebSocketMessage::pack(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    return m_message->pack(data, retVal);
}

result_t WebSocketMessage::pack(v8::Local<v8::Value>& retVal)
{
    return m_message->pack(retVal);
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
    m_message = new Message();
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
            next(read);
        }

        ON_STATE(asyncCopy, read)
        {
            int64_t len;

            if (m_bytes == 0)
                return next();

            if (m_bytes > STREAM_BUFF_SIZE)
                len = STREAM_BUFF_SIZE;
            else
                len = m_bytes;

            m_buf.Release();
            return m_from->read((int32_t)len, m_buf, next(write));
        }

        ON_STATE(asyncCopy, write)
        {
            int32_t blen;

            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));

            if (m_mask != 0) {
                exlib::string strBuffer;
                int32_t i, n;
                uint8_t* mask = (uint8_t*)&m_mask;

                m_buf->toString(strBuffer);

                n = (int32_t)strBuffer.length();
                char* _strBuffer = strBuffer.c_buffer();
                for (i = 0; i < n; i++)
                    _strBuffer[i] ^= mask[(m_copyed + i) & 3];

                m_buf = new Buffer(strBuffer);
            }

            m_buf->get_length(blen);
            m_copyed += blen;

            if (m_bytes > 0)
                m_bytes -= blen;

            return m_to->write(m_buf, next(read));
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

result_t WebSocketMessage::sendTo(Stream_base* stm, WebSocket* wss, AsyncEvent* ac)
{
    class asyncSendTo : public AsyncState {
    public:
        asyncSendTo(WebSocketMessage* pThis, Stream_base* stm, WebSocket* wss, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_wss(wss)
            , m_mask(0)
            , m_take_over(false)
        {
            m_pThis->get_body(m_body);

            if (m_pThis->m_compress) {
                m_data = new MemoryStream();

                if (m_wss && m_wss->m_compress) {
                    m_take_over = true;
                    m_wss->m_deflate->attach(m_data);
                    m_zip = m_wss->m_deflate;
                } else
                    zlib_base::createDeflateRaw(m_data, m_zip);

                next(deflate);
            } else {
                m_data = m_body;
                next(head);
            }
        }

        ON_STATE(asyncSendTo, deflate)
        {
            m_body->rewind();
            return m_body->copyTo(m_zip, -1, m_size, next(flush));
        }

        ON_STATE(asyncSendTo, flush)
        {
            return m_zip->flush(next(head));
        }

        ON_STATE(asyncSendTo, head)
        {
            if (m_take_over)
                m_wss->m_deflate->attach(NULL);

            int64_t size;
            m_data->size(size);
            if (m_pThis->m_compress)
                size -= 4;
            m_size = size;

            uint8_t buf[16];
            int32_t pos = 0;

            int32_t type;
            m_pThis->get_type(type);
            if (m_pThis->m_compress)
                buf[0] = 0xc0 | (type & 0x0f);
            else
                buf[0] = 0x80 | (type & 0x0f);

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

            if (m_pThis->m_masked) {
                buf[1] |= 0x80;

                uint32_t r = 0;
                while (r == 0)
                    r = rand();

                m_mask = r;

                *(uint32_t*)(buf + pos) = r;
                pos += 4;
            }

            m_buffer = new Buffer((const char*)buf, pos);
            return m_stm->write(m_buffer, next(sendData));
        }

        ON_STATE(asyncSendTo, sendData)
        {
            m_data->rewind();
            return copy(m_data, m_stm, m_size, m_mask, next());
        }

        virtual int32_t error(int32_t v)
        {
            if (m_take_over)
                m_wss->m_deflate->attach(NULL);
            return v;
        }

    public:
        obj_ptr<Stream_base> m_zip;
        obj_ptr<SeekableStream_base> m_data;
        obj_ptr<WebSocketMessage> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<WebSocket> m_wss;
        obj_ptr<SeekableStream_base> m_body;
        int64_t m_size;
        uint32_t m_mask;
        obj_ptr<Buffer_base> m_buffer;
        bool m_take_over;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSendTo(this, stm, wss, ac))->post(0);
}

result_t WebSocketMessage::sendTo(Stream_base* stm, AsyncEvent* ac)
{
    return sendTo(stm, NULL, ac);
}

result_t WebSocketMessage::readFrom(Stream_base* stm, WebSocket* wss, AsyncEvent* ac)
{
    class asyncReadFrom : public AsyncState {
    public:
        asyncReadFrom(WebSocketMessage* pThis, Stream_base* stm, WebSocket* wss, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_wss(wss)
            , m_fin(false)
            , m_masked(false)
            , m_fragmented(false)
            , m_size(0)
            , m_fullsize(0)
            , m_mask(0)
            , m_take_over(false)
        {
            m_pThis->get_body(m_body);
            m_zip = m_body;
            next(head);
        }

        ON_STATE(asyncReadFrom, head)
        {
            return m_stm->read(2, m_buffer, next(extHead));
        }

        ON_STATE(asyncReadFrom, extHead)
        {
            if (n == CALL_RETURN_NULL) {
                m_pThis->m_error = 1001;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
            }

            exlib::string strBuffer;
            char ch;
            int32_t sz = 0;

            m_buffer->toString(strBuffer);
            m_buffer.Release();

            ch = strBuffer[0];
            if (ch & 0x40) {
                if (m_wss && m_wss->m_compress) {
                    m_take_over = true;
                    m_wss->m_inflate->attach(m_body);
                    m_zip = m_wss->m_inflate;
                } else
                    zlib_base::createInflateRaw(m_body, m_pThis->m_maxSize, m_zip);

                m_pThis->m_compress = true;
            } else if (ch & 0x70) {
                m_pThis->m_error = 1007;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: non-zero RSV values."));
            }

            m_fin = (ch & 0x80) != 0;

            if (m_fragmented) {
                if ((ch & 0x0f) != ws_base::C_CONTINUE) {
                    m_pThis->m_error = 1007;
                    return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
                }
            } else
                m_pThis->set_type(ch & 0x0f);

            ch = strBuffer[1];

            if (m_fragmented) {
                if (m_masked != (m_pThis->m_masked = (ch & 0x80) != 0)) {
                    m_pThis->m_error = 1007;
                    return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
                }
            } else
                m_masked = m_pThis->m_masked = (ch & 0x80) != 0;

            if (m_masked)
                sz += 4;

            m_size = ch & 0x7f;
            if (m_size == 126)
                sz += 2;
            else if (m_size == 127)
                sz += 8;

            if (sz)
                return m_stm->read(sz, m_buffer, next(extReady));

            return next(copy);
        }

        ON_STATE(asyncReadFrom, extReady)
        {
            if (n == CALL_RETURN_NULL) {
                m_pThis->m_error = 1007;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: payload processing failed."));
            }

            exlib::string strBuffer;
            int32_t pos = 0;

            m_buffer->toString(strBuffer);
            m_buffer.Release();

            if (m_size == 126) {
                m_size = ((uint32_t)(uint8_t)strBuffer[0] << 8) + (uint8_t)strBuffer[1];
                pos += 2;
            } else if (m_size == 127) {
                m_size = ((int64_t)(uint8_t)strBuffer[0] << 56) + ((int64_t)(uint8_t)strBuffer[1] << 48) + ((int64_t)(uint8_t)strBuffer[2] << 40) + ((int64_t)(uint8_t)strBuffer[3] << 32) + ((int64_t)(uint8_t)strBuffer[4] << 24) + ((int64_t)(uint8_t)strBuffer[5] << 16) + ((int64_t)(uint8_t)strBuffer[6] << 8) + (int64_t)(uint8_t)strBuffer[7];
                pos += 8;
            }

            if (m_masked)
                memcpy(&m_mask, strBuffer.c_str() + pos, 4);

            return next(copy);
        }

        ON_STATE(asyncReadFrom, copy)
        {
            if (m_fullsize + m_size > m_pThis->m_maxSize) {
                m_pThis->m_error = 1009;
                return CHECK_ERROR(Runtime::setError("WebSocketMessage: Message Too Big."));
            }

            return WebSocketMessage::copy(m_stm, m_zip, m_size, m_mask, next(copy_end));
        }

        ON_STATE(asyncReadFrom, copy_end)
        {
            if (!m_fin) {
                m_fragmented = true;
                m_mask = 0;
                m_fullsize += m_size;
                return next(head);
            }

            if (m_take_over)
                return m_zip->write(m_wss->m_flushTail, next(tail_end));

            return m_zip->flush(next(body_end));
        }

        ON_STATE(asyncReadFrom, tail_end)
        {
            return m_zip->flush(next(body_end));
        }

        ON_STATE(asyncReadFrom, body_end)
        {
            if (m_take_over)
                m_wss->m_inflate->attach(NULL);

            m_body->rewind();
            return next();
        }

        virtual int32_t error(int32_t v)
        {
            if (m_take_over)
                m_wss->m_inflate->attach(NULL);
            return v;
        }

    public:
        obj_ptr<WebSocketMessage> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<WebSocket> m_wss;
        obj_ptr<Stream_base> m_zip;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Buffer_base> m_buffer;
        bool m_fin;
        bool m_masked;
        bool m_fragmented;
        int64_t m_size;
        int64_t m_fullsize;
        uint32_t m_mask;
        bool m_take_over;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_stm = stm;

    return (new asyncReadFrom(this, stm, wss, ac))->post(0);
}

result_t WebSocketMessage::readFrom(Stream_base* stm, AsyncEvent* ac)
{
    return readFrom(stm, NULL, ac);
}

result_t WebSocketMessage::get_stream(obj_ptr<Stream_base>& retVal)
{
    if (!m_stm)
        return CALL_RETURN_NULL;

    retVal = m_stm;
    return 0;
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

result_t WebSocketMessage::get_compress(bool& retVal)
{
    retVal = m_compress;
    return 0;
}

result_t WebSocketMessage::set_compress(bool newVal)
{
    m_compress = newVal;
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
