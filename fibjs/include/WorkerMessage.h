/*
 * WorkerMessage.h
 *
 *  Created on: Apr 26, 2017
 *      Author: lion
 */

#include "Message.h"

#ifndef WORKERMESSAGE_H_
#define WORKERMESSAGE_H_

namespace fibjs {

class WorkerMessage : public Message_base {
public:
    WorkerMessage(v8::Local<v8::Value> v)
        : m_v(v)
    {
        m_message = new Message();
    }

public:
    // Message_base
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_params(obj_ptr<List_base>& retVal);
    virtual result_t set_params(List_base* newVal);
    virtual result_t get_type(int32_t& retVal);
    virtual result_t set_type(int32_t newVal);
    virtual result_t get_data(v8::Local<v8::Value>& retVal);
    virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal);
    virtual result_t set_body(SeekableStream_base* newVal);
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, AsyncEvent* ac);
    virtual result_t json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal);
    virtual result_t json(v8::Local<v8::Value>& retVal);
    virtual result_t get_length(int64_t& retVal);
    virtual result_t end();
    virtual result_t isEnded(bool& retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base* stm, AsyncEvent* ac);
    virtual result_t readFrom(Stream_base* stm, AsyncEvent* ac);
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal);
    virtual result_t get_response(obj_ptr<Message_base>& retVal);
    virtual result_t get_lastError(exlib::string& retVal);
    virtual result_t set_lastError(exlib::string newVal);

public:
    result_t unbind()
    {
        return m_v.unbind();
    }

private:
    obj_ptr<Message> m_message;
    Variant m_v;
};

} /* namespace fibjs */
#endif /* WORKERMESSAGE_H_ */
