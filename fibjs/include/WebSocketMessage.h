/*
 * WebSocketMessage.h
 *
 *  Created on: Sep 9, 2015
 *      Author: lion
 */

#pragma once

#include "Message.h"
#include "ifs/WebSocketMessage.h"
#include "WebSocket.h"

namespace fibjs {

class WebSocketMessage : public WebSocketMessage_base {
public:
    WebSocketMessage(int32_t type, bool masked, bool compress, int32_t maxSize)
        : m_masked(masked)
        , m_compress(compress)
        , m_maxSize(maxSize)
        , m_error(0)
    {
        if (type != ws_base::C_TEXT && type != ws_base::C_BINARY)
            m_compress = false;

        m_message = new Message();
        m_message->set_type(type);
    }

public:
    // Message_base
    virtual result_t get_sent(bool& retVal);
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_params(obj_ptr<NArray>& retVal);
    virtual result_t get_type(int32_t& retVal);
    virtual result_t set_type(int32_t newVal);
    virtual result_t get_data(v8::Local<v8::Value>& retVal);
    virtual result_t get_body(obj_ptr<Stream_base>& retVal);
    virtual result_t set_body(Stream_base* newVal);
    virtual result_t get_bodyUsed(bool& retVal) { return m_message->get_bodyUsed(retVal); }
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
    virtual result_t clone(obj_ptr<Message_base>& retVal);

public:
    // WebSocketMessage_base
    virtual result_t get_masked(bool& retVal);
    virtual result_t set_masked(bool newVal);
    virtual result_t get_compress(bool& retVal);
    virtual result_t set_compress(bool newVal);
    virtual result_t get_maxSize(int32_t& retVal);
    virtual result_t set_maxSize(int32_t newVal);

public:
    static result_t copy(Stream_base* from, Stream_base* to, int64_t bytes, uint32_t mask, AsyncEvent* ac);
    result_t sendTo(Stream_base* stm, WebSocket* wss, AsyncEvent* ac);
    result_t readFrom(Stream_base* stm, WebSocket* wss, AsyncEvent* ac);

public:
    obj_ptr<Stream_base> m_stm;
    bool m_masked;
    bool m_compress;
    int32_t m_maxSize;
    int32_t m_error;

private:
    obj_ptr<Message> m_message;
};

} /* namespace fibjs */
