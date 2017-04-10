/*
 * HttpResponse.h
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#include "ifs/HttpResponse.h"
#include "HttpMessage.h"

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

namespace fibjs {

class HttpResponse : public HttpResponse_base {

public:
    HttpResponse()
    {
        m_message = new HttpMessage(true);
        clear();
        extMemory(4096);
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
    // HttpMessage_base
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t set_protocol(exlib::string newVal);
    virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
    virtual result_t get_keepAlive(bool& retVal);
    virtual result_t set_keepAlive(bool newVal);
    virtual result_t get_upgrade(bool& retVal);
    virtual result_t set_upgrade(bool newVal);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxUploadSize(int32_t& retVal);
    virtual result_t set_maxUploadSize(int32_t newVal);
    virtual result_t hasHeader(exlib::string name, bool& retVal);
    virtual result_t firstHeader(exlib::string name, Variant& retVal);
    virtual result_t allHeader(exlib::string name, obj_ptr<List_base>& retVal);
    virtual result_t addHeader(Map_base* map);
    virtual result_t addHeader(exlib::string name, Variant value);
    virtual result_t setHeader(Map_base* map);
    virtual result_t setHeader(exlib::string name, Variant value);
    virtual result_t removeHeader(exlib::string name);

public:
    // HttpResponse_base
    virtual result_t get_status(int32_t& retVal);
    virtual result_t set_status(int32_t newVal);
    virtual result_t get_cookies(obj_ptr<List_base>& retVal);
    virtual result_t addCookie(HttpCookie_base* cookie);
    virtual result_t redirect(exlib::string url);
    virtual result_t sendHeader(Stream_base* stm, AsyncEvent* ac);

public:
    obj_ptr<HttpMessage> m_message;
    int32_t m_status;
    obj_ptr<List_base> m_cookies;
};

} /* namespace fibjs */
#endif /* HTTPRESPONSE_H_ */
