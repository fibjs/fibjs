/*
 * _Message.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "Message.h"
#include "List.h"
#include "MemoryStream.h"

namespace fibjs {

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

result_t Message::get_params(obj_ptr<List_base>& retVal)
{
    if (m_params == NULL)
        m_params = new List();

    retVal = m_params;
    return 0;
}

result_t Message::set_params(List_base* newVal)
{
    m_params = newVal;
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
    result_t hr;

    obj_ptr<SeekableStream_base> body;
    obj_ptr<Buffer_base> data;

    hr = get_body(body);
    if (hr < 0)
        return hr;

    body->rewind();
    hr = body->cc_readAll(data);
    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_UNDEFINED)
        return CALL_RETURN_UNDEFINED;

    if (m_type == _TEXT) {
        exlib::string txt;

        data->toString(txt);
        retVal = holder()->NewFromUtf8(txt);
    } else
        retVal = data->wrap();

    return 0;
}

result_t Message::get_body(obj_ptr<SeekableStream_base>& retVal)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

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
        return CALL_RETURN_UNDEFINED;

    return m_body->read(bytes, retVal, ac);
}

result_t Message::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (m_body == NULL)
        return CALL_RETURN_UNDEFINED;

    return m_body->readAll(retVal, ac);
}

result_t Message::write(Buffer_base* data, AsyncEvent* ac)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    return m_body->write(data, ac);
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

result_t Message::end()
{
    m_end = true;
    return 0;
}

result_t Message::isEnded(bool& retVal)
{
    retVal = m_end;
    if (!m_end && m_response)
        m_response->isEnded(retVal);

    return 0;
}

result_t Message::clear()
{
    m_end = false;
    m_params.Release();
    m_value.clear();
    m_body.Release();

    if (m_response)
        m_response->clear();

    return 0;
}

result_t Message::sendTo(Stream_base* stm, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::readFrom(Stream_base* stm, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::get_stream(obj_ptr<Stream_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Message::get_response(obj_ptr<Message_base>& retVal)
{
    if (m_bRep)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!m_response)
        m_response = new Message(true);

    retVal = m_response;
    return 0;
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

} /* namespace fibjs */
