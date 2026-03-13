/*
 * Message.h
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Message.h"
#include "ifs/SeekableStream.h"
#include "MemoryStream.h"

namespace fibjs {

class Message : public Message_base {
public:
    // Message_base
    virtual result_t get_sent(bool& retVal);
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_params(obj_ptr<NArray>& retVal);
    virtual result_t get_type(int32_t& retVal);
    virtual result_t set_type(int32_t newVal);
    virtual result_t get_data(v8::Local<v8::Value>& retVal);
    virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal);
    virtual result_t set_body(SeekableStream_base* newVal);
    virtual result_t get_bodyUsed(bool& retVal);
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac);
    virtual result_t text(exlib::string data, exlib::string& retVal);
    virtual result_t text(exlib::string& retVal);
    virtual result_t arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal);
    virtual result_t json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal);
    virtual result_t json(v8::Local<v8::Value>& retVal);
    virtual result_t pack(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal);
    virtual result_t pack(v8::Local<v8::Value>& retVal);
    virtual result_t get_length(int64_t& retVal);
    virtual result_t end(int32_t& retVal, AsyncEvent* ac);
    virtual result_t end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac);
    virtual result_t end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac);
    virtual result_t end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac);
    virtual result_t isEnded(bool& retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac);
    virtual result_t readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac);
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal);
    virtual result_t get_lastError(exlib::string& retVal);
    virtual result_t set_lastError(exlib::string newVal);

public:
    Message()
        : m_type(C_BINARY)
        , m_end(false)
    {
    }

public:
    obj_ptr<SeekableStream_base>& body()
    {
        return m_body;
    }

    // Ensure body stream exists for writing, creates MemoryStream if needed
    obj_ptr<SeekableStream_base>& ensure_body()
    {
        if (m_body == NULL)
            m_body = new MemoryStream();
        return m_body;
    }

    // Clone this message - must be implemented by concrete subclasses
    virtual result_t clone(obj_ptr<Message_base>& retVal);

    // Helper method to copy base Message properties to another Message instance
    void copyTo(Message* target);
    int32_t m_type;
    obj_ptr<NArray> m_params;
    exlib::string m_value;
    obj_ptr<SeekableStream_base> m_body;
    exlib::string m_lastError;
    bool m_end;
    bool m_bodyUsed = false;
};

} /* namespace fibjs */
