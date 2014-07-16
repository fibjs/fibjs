/*
 * _Message.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "Message.h"
#include "List.h"
#include "MemoryStream.h"

namespace fibjs
{

result_t Message::_msg::get_value(std::string &retVal)
{
    retVal = m_value;
    return 0;
}

result_t Message::_msg::set_value(const char *newVal)
{
    m_value = newVal;
    return 0;
}

result_t Message::_msg::get_params(obj_ptr<List_base> &retVal)
{
    if (m_values == NULL)
        m_values = new values();

    if (m_values->m_params == NULL)
        m_values->m_params = new List();

    retVal = m_values->m_params;
    return 0;
}

result_t Message::_msg::set_params(List_base *newVal)
{
    if (m_values == NULL)
        m_values = new values();

    m_values->m_params = newVal;
    return 0;
}

result_t Message::_msg::get_result(Variant &retVal)
{
    if (m_values == NULL)
        m_values = new values();

    retVal = m_values->m_result;
    return 0;
}

result_t Message::_msg::set_result(Variant newVal)
{
    if (m_values == NULL)
        m_values = new values();

    m_values->m_result = newVal;
    return 0;
}

result_t Message::_msg::get_body(obj_ptr<SeekableStream_base> &retVal)
{
    if (m_body == NULL)
        m_body = new MemoryStream();

    retVal = m_body;
    return 0;
}

result_t Message::_msg::set_body(SeekableStream_base *newVal)
{
    m_body = newVal;
    return 0;
}

result_t Message::_msg::get_length(int64_t &retVal)
{
    if (m_body == NULL)
    {
        retVal = 0;
        return 0;
    }
    return m_body->size(retVal);
}

result_t Message_base::_new(obj_ptr<Message_base> &retVal)
{
    retVal = new Message();
    return 0;
}

result_t Message::get_value(std::string &retVal)
{
    return m_message.get_value(retVal);
}

result_t Message::set_value(const char *newVal)
{
    return m_message.set_value(newVal);
}

result_t Message::get_params(obj_ptr<List_base> &retVal)
{
    return m_message.get_params(retVal);
}

result_t Message::set_params(List_base *newVal)
{
    return m_message.set_params(newVal);
}

result_t Message::get_result(Variant &retVal)
{
    return m_message.get_result(retVal);
}

result_t Message::set_result(Variant newVal)
{
    return m_message.set_result(newVal);
}

result_t Message::get_body(obj_ptr<SeekableStream_base> &retVal)
{
    return m_message.get_body(retVal);
}

result_t Message::set_body(SeekableStream_base *newVal)
{
    return m_message.set_body(newVal);
}

result_t Message::get_length(int64_t &retVal)
{
    return m_message.get_length(retVal);
}

result_t Message::clear()
{
    m_message.clear();
    return 0;
}

result_t Message::sendTo(Stream_base *stm, exlib::AsyncEvent *ac)
{
    return CALL_E_INVALID_CALL;
}

result_t Message::readFrom(BufferedStream_base *stm, exlib::AsyncEvent *ac)
{
    return CALL_E_INVALID_CALL;
}

result_t Message::get_stream(obj_ptr<Stream_base> &retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Message::get_response(obj_ptr<Message_base> &retVal)
{
    if (m_bRep)
        return CALL_E_INVALID_CALL;

    if (!m_response)
        m_response = new Message(true);

    retVal = m_response;
    return 0;
}

} /* namespace fibjs */
