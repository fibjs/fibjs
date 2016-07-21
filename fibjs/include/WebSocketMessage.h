/*
 * WebSocketMessage.h
 *
 *  Created on: Sep 9, 2015
 *      Author: lion
 */

#include "Message.h"
#include "ifs/WebSocketMessage.h"

#ifndef WEBSOCKETMESSAGE_H_
#define WEBSOCKETMESSAGE_H_

namespace fibjs
{

class WebSocketMessage: public WebSocketMessage_base
{
public:
    WebSocketMessage(int32_t type, bool masked, int32_t maxSize, bool bRep = false)
        : m_type(type), m_masked(masked), m_maxSize(maxSize), m_bRep(bRep)
    {
        m_message = new Message();
    }

public:
    // Message_base
    virtual result_t get_value(exlib::string &retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_params(obj_ptr<List_base> &retVal);
    virtual result_t set_params(List_base *newVal);
    virtual result_t get_result(Variant &retVal);
    virtual result_t set_result(Variant newVal);
    virtual result_t get_body(obj_ptr<SeekableStream_base> &retVal);
    virtual result_t set_body(SeekableStream_base *newVal);
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t readAll(obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, AsyncEvent *ac);
    virtual result_t get_length(int64_t &retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base *stm, AsyncEvent *ac);
    virtual result_t readFrom(Stream_base *stm, AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_response(obj_ptr<Message_base> &retVal);
    virtual result_t get_lastError(exlib::string& retVal);
    virtual result_t set_lastError(exlib::string newVal);

public:
    // WebSocketMessage_base
    virtual result_t get_type(int32_t& retVal);
    virtual result_t set_type(int32_t newVal);
    virtual result_t get_masked(bool& retVal);
    virtual result_t set_masked(bool newVal);
    virtual result_t get_maxSize(int32_t& retVal);
    virtual result_t set_maxSize(int32_t newVal);

public:
    static result_t copy(Stream_base *from, Stream_base *to, int64_t bytes, uint32_t mask, AsyncEvent *ac);

public:
    obj_ptr<Stream_base> m_stm;
    int32_t m_type;
    bool m_masked;
    int32_t m_maxSize;

private:
    obj_ptr<Message> m_message;
    obj_ptr<WebSocketMessage_base> m_response;
    bool m_bRep;
};

} /* namespace fibjs */
#endif /* WEBSOCKETMESSAGE_H_ */
