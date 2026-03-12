/*
 * _Message.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "Message.h"
#include "MemoryStream.h"
#include "Buffer.h"
#include "ifs/json.h"
#include "ifs/msgpack.h"
#include "ifs/fs.h"
#include "v8_api.h"

namespace fibjs {

result_t Message::get_sent(bool& retVal)
{
    retVal = false;
    return 0;
}

result_t Message::get_value(exlib::string& retVal)
{
    retVal = m_value;
    return 0;
}

result_t Message::set_value(exlib::string newVal)
{
    m_value = newVal;
    return 0;
}

result_t Message::get_params(obj_ptr<NArray>& retVal)
{
    if (m_params == NULL)
        m_params = new NArray();

    retVal = m_params;
    return 0;
}

result_t Message::get_type(int32_t& retVal)
{
    retVal = m_type;
    return 0;
}

result_t Message::set_type(int32_t newVal)
{
    m_type = newVal;
    return 0;
}

result_t Message::get_data(v8::Local<v8::Value>& retVal)
{
    if (m_body == NULL)
        return CALL_RETURN_NULL;

    result_t hr;
    obj_ptr<Buffer_base> data;

    m_body->rewind();
    hr = m_body->ac_readAll(data);
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL)
        return CALL_RETURN_NULL;

    if (m_type == C_TEXT) {
        exlib::string txt;

        data->toString(txt);
        retVal = holder()->NewString(txt);
    } else
        return data->valueOf(retVal);

    return 0;
}

result_t Message::get_body(obj_ptr<SeekableStream_base>& retVal)
{
    if (m_body == NULL)
        return CALL_RETURN_NULL;

    // Return null for empty body (Web API compatibility)
    int64_t size = 0;
    m_body->size(size);
    if (size == 0)
        return CALL_RETURN_NULL;

    retVal = m_body;
    return 0;
}

result_t Message::set_body(SeekableStream_base* newVal)
{
    m_body = newVal;
    return 0;
}

result_t Message::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (m_body == NULL)
        return CALL_RETURN_NULL;

    return m_body->readBuffer(bytes, retVal, ac);
}

result_t Message::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        return CALL_RETURN_NULL;

    return m_body->readAll(retVal, ac);
}

result_t Message::write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    bool _retVal;
    return m_body->write(data, _retVal, ac);
}

result_t Message::text(exlib::string data, exlib::string& retVal)
{
    m_body = new MemoryStream();

    obj_ptr<Buffer_base> buf = new Buffer(data.c_str(), data.length());
    bool len;
    return m_body->ac_write(buf, len);
}

result_t Message::text(exlib::string& retVal)
{
    if (m_body == NULL) {
        retVal = "";
        return 0;
    }

    result_t hr;
    obj_ptr<Buffer_base> data;

    m_body->rewind();
    hr = m_body->ac_readAll(data);
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL) {
        retVal = "";
        return 0;
    }

    return data->toString(retVal);
}

result_t Message::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal)
{
    if (m_body == NULL) {
        retVal = NewBackingStore(0);
        return 0;
    }

    result_t hr;
    obj_ptr<Buffer_base> data;

    m_body->rewind();
    hr = m_body->ac_readAll(data);
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL) {
        retVal = NewBackingStore(0);
        return 0;
    }

    Buffer* buf = data.As<Buffer>();
    int32_t bufSize = buf->length();
    const uint8_t* bufData = buf->data();

    std::shared_ptr<v8::BackingStore> store = NewBackingStore(bufSize);
    if (bufSize > 0 && store->Data() && bufData) {
        memcpy(store->Data(), bufData, bufSize);
    }

    retVal = std::move(store);
    return 0;
}

result_t Message::json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    m_body = new MemoryStream();

    exlib::string str;
    result_t hr = json_base::encode(data, str);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(str.c_str(), str.length());
    bool len;
    return m_body->ac_write(buf, len);
}

result_t Message::json(v8::Local<v8::Value>& retVal)
{
    if (m_body == NULL)
        return CALL_RETURN_NULL;

    result_t hr;
    obj_ptr<Buffer_base> data;

    m_body->rewind();
    hr = m_body->ac_readAll(data);
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL)
        return CALL_RETURN_NULL;

    exlib::string str;
    data->toString(str);

    return json_base::decode(str, retVal);
}

result_t Message::pack(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal)
{
    m_body = new MemoryStream();

    obj_ptr<Buffer_base> buf;
    result_t hr = msgpack_base::encode(data, buf);
    if (hr < 0)
        return hr;

    bool len;
    return m_body->ac_write(buf, len);
}

result_t Message::pack(v8::Local<v8::Value>& retVal)
{
    if (m_body == NULL)
        return CALL_RETURN_NULL;

    result_t hr;
    obj_ptr<Buffer_base> data;

    m_body->rewind();
    hr = m_body->ac_readAll(data);
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL)
        return CALL_RETURN_NULL;

    return msgpack_base::decode(data, retVal);
}

result_t Message::get_length(int64_t& retVal)
{
    if (m_body == NULL) {
        retVal = 0;
        return 0;
    }
    return m_body->size(retVal);
}

result_t Message_base::_new(obj_ptr<Message_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Message();
    return 0;
}

result_t Message::end(int32_t& retVal, AsyncEvent* ac)
{
    m_end = true;
    retVal = 0;
    return 0;
}

result_t Message::end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    m_end = true;
    return m_body->end(data, retVal, ac);
}

result_t Message::end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    m_end = true;
    return m_body->end(data, encoding, retVal, ac);
}

result_t Message::end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    m_end = true;
    return m_body->end(data, encoding, retVal, ac);
}

result_t Message::isEnded(bool& retVal)
{
    retVal = m_end;
    return 0;
}

result_t Message::clear()
{
    m_end = false;
    m_params.Release();
    m_value.clear();
    m_body.Release();

    return 0;
}

result_t Message::sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::get_stream(obj_ptr<Stream_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::get_lastError(exlib::string& retVal)
{
    retVal = m_lastError;
    return 0;
}

result_t Message::set_lastError(exlib::string newVal)
{
    m_lastError = newVal;
    return 0;
}

result_t Message::clone(obj_ptr<Message_base>& retVal)
{
    // Message is an abstract base class, cannot be cloned directly
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

void Message::copyTo(Message* target)
{
    target->m_type = m_type;
    target->m_value = m_value;
    target->m_lastError = m_lastError;
    target->m_end = m_end;

    // Clone params array
    if (m_params) {
        target->m_params = new NArray();
        int32_t len = m_params->length();
        for (int32_t i = 0; i < len; i++) {
            Variant v;
            m_params->_indexed_getter(i, v);
            target->m_params->append(v);
        }
    }

    // Clone body stream
    if (m_body) {
        int64_t pos;
        m_body->tell(pos);
        m_body->rewind();

        obj_ptr<Buffer_base> buf;
        result_t hr = m_body->ac_readAll(buf);

        m_body->seek(pos, fs_base::C_SEEK_SET);

        if (hr >= 0 && buf) {
            obj_ptr<MemoryStream> ms = new MemoryStream();
            ms->writeBuffer(buf, nullptr);
            ms->rewind();
            target->m_body = ms;
        }
    }
}

} /* namespace fibjs */
