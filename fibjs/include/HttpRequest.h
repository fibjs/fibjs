/*
 * HttpRequest.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/HttpRequest.h"
#include "HttpMessage.h"
#include "HttpResponse.h"
#include "Headers.h"
#include "HttpCollection.h"

namespace fibjs {

class HttpRequest : public HttpRequest_base {
public:
    HttpRequest()
    {
        m_message = new HttpMessage();
        clear();
        extMemory(4096);
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
    virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal);
    virtual result_t set_body(SeekableStream_base* newVal);
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
    virtual result_t end();
    virtual result_t isEnded(bool& retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac);
    virtual result_t readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac);
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal);
    virtual result_t get_lastError(exlib::string& retVal);
    virtual result_t set_lastError(exlib::string newVal);
    virtual result_t clone(obj_ptr<Message_base>& retVal);

public:
    // HttpMessage_base
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t set_protocol(exlib::string newVal);
    virtual result_t get_headers(obj_ptr<Headers_base>& retVal);
    virtual result_t get_keepAlive(bool& retVal);
    virtual result_t set_keepAlive(bool newVal);
    virtual result_t get_upgrade(bool& retVal);
    virtual result_t set_upgrade(bool newVal);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxHeaderSize(int32_t& retVal);
    virtual result_t set_maxHeaderSize(int32_t newVal);
    virtual result_t get_maxChunkSize(int32_t& retVal);
    virtual result_t set_maxChunkSize(int32_t newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_socket(obj_ptr<Stream_base>& retVal);
    virtual result_t hasHeader(exlib::string name, bool& retVal);
    virtual result_t firstHeader(exlib::string name, exlib::string& retVal);
    virtual result_t allHeader(exlib::string name, obj_ptr<NObject>& retVal);
    virtual result_t appendHeader(v8::Local<v8::Object> map);
    virtual result_t appendHeader(Headers_base* headers);
    virtual result_t appendHeader(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t appendHeader(exlib::string name, exlib::string value);
    virtual result_t setHeader(v8::Local<v8::Object> map);
    virtual result_t setHeader(Headers_base* headers);
    virtual result_t setHeader(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t setHeader(exlib::string name, exlib::string value);
    virtual result_t removeHeader(exlib::string name);

public:
    // HttpRequest_base
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal);
    virtual result_t get_method(exlib::string& retVal);
    virtual result_t set_method(exlib::string newVal);
    virtual result_t get_address(exlib::string& retVal);
    virtual result_t set_address(exlib::string newVal);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_queryString(exlib::string& retVal);
    virtual result_t set_queryString(exlib::string newVal);
    virtual result_t get_cookies(obj_ptr<HttpCollection_base>& retVal);
    virtual result_t get_form(obj_ptr<FormData_base>& retVal);
    virtual result_t get_query(obj_ptr<URLSearchParams_base>& retVal);

public:
    void _appendHeader(exlib::string name, exlib::string value)
    {
        if (!qstricmp(name.c_str(), "connection")) {
            if (qstristr(value.c_str(), "keep-alive")) {
                m_message->set_keepAlive(true);
                return;
            } else if (qstristr(value.c_str(), "close")) {
                m_message->set_keepAlive(false);
                return;
            }
        }

        m_message->appendHeader(name, value);
    }

private:
    obj_ptr<HttpResponse_base> m_response;
    obj_ptr<HttpMessage> m_message;
    exlib::string m_method;
    exlib::string m_address;
    exlib::string m_queryString;
    obj_ptr<HttpCollection_base> m_cookies;
    obj_ptr<URLSearchParams_base> m_query;
    obj_ptr<FormData_base> m_form;
};

} /* namespace fibjs */
