/*
 * WorkerMessage.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: lion
 */

#include "object.h"
#include "WorkerMessage.h"

namespace fibjs {

result_t WorkerMessage::get_value(exlib::string& retVal)
{
    return m_message->get_value(retVal);
}

result_t WorkerMessage::set_value(exlib::string newVal)
{
    return m_message->set_value(newVal);
}

result_t WorkerMessage::get_params(obj_ptr<List_base>& retVal)
{
    return m_message->get_params(retVal);
}

result_t WorkerMessage::set_params(List_base* newVal)
{
    return m_message->set_params(newVal);
}

result_t WorkerMessage::get_body(obj_ptr<SeekableStream_base>& retVal)
{
    return m_message->get_body(retVal);
}

result_t WorkerMessage::set_body(SeekableStream_base* newVal)
{
    return m_message->set_body(newVal);
}

result_t WorkerMessage::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    return m_message->read(bytes, retVal, ac);
}

result_t WorkerMessage::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return m_message->readAll(retVal, ac);
}

result_t WorkerMessage::write(Buffer_base* data, AsyncEvent* ac)
{
    return m_message->write(data, ac);
}

result_t WorkerMessage::json(v8::Local<v8::Value> data, AsyncEvent* ac)
{
    return m_message->json(data, ac);
}

result_t WorkerMessage::get_length(int64_t& retVal)
{
    return m_message->get_length(retVal);
}

result_t WorkerMessage::get_lastError(exlib::string& retVal)
{
    return m_message->get_lastError(retVal);
}

result_t WorkerMessage::set_lastError(exlib::string newVal)
{
    return m_message->set_lastError(newVal);
}

result_t WorkerMessage::end()
{
    return m_message->end();
}

result_t WorkerMessage::isEnded(bool& retVal)
{
    return m_message->isEnded(retVal);
}

result_t WorkerMessage::clear()
{
    m_message = new Message();
    return 0;
}

result_t WorkerMessage::get_stream(obj_ptr<Stream_base>& retVal)
{
    return m_message->get_stream(retVal);
}

result_t WorkerMessage::get_response(obj_ptr<Message_base>& retVal)
{
    return m_message->get_response(retVal);
}

result_t WorkerMessage::get_type(int32_t& retVal)
{
    return m_message->get_type(retVal);
}

result_t WorkerMessage::set_type(int32_t newVal)
{
    return m_message->set_type(newVal);
}

result_t WorkerMessage::get_data(v8::Local<v8::Value>& retVal)
{
    retVal = m_v;
    return 0;
}

result_t WorkerMessage::sendTo(Stream_base* stm, AsyncEvent* ac)
{
    return m_message->sendTo(stm, ac);
}

result_t WorkerMessage::readFrom(Stream_base* stm, AsyncEvent* ac)
{
    return m_message->readFrom(stm, ac);
}

} /* namespace fibjs */
