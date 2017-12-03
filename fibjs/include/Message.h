/*
 * Message.h
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "ifs/Message.h"
#include "ifs/SeekableStream.h"

#ifndef MESSAGE_H_
#define MESSAGE_H_

namespace fibjs {

class Message : public Message_base {
public:
    // Message_base
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_params(obj_ptr<NArray>& retVal);
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
    virtual result_t get_lastError(exlib::string& retVal);
    virtual result_t set_lastError(exlib::string newVal);

public:
    Message(bool bRep = false)
        : m_type(_BINARY)
        , m_bRep(bRep)
        , m_end(false)
    {
    }

public:
    obj_ptr<SeekableStream_base>& body()
    {
        return m_body;
    }

private:
    int32_t m_type;
    obj_ptr<NArray> m_params;
    exlib::string m_value;
    obj_ptr<SeekableStream_base> m_body;
    exlib::string m_lastError;
    bool m_bRep;
    bool m_end;
};

} /* namespace fibjs */
#endif /* MESSAGE_H_ */
