/*
 * HttpResponse.h
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/HttpResponse.h"
#include "HttpMessage.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

class HttpResponse : public HttpResponse_base {

public:
    class Options : public obj_base {
    public:
        LOAD_OPTIONS(Options, (header_only)(content_length));

    public:
        std::optional<bool> header_only = false;
        std::optional<bool> content_length;
    };

public:
    HttpResponse()
    {
        m_message = new HttpMessage(true);
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

public:
    // HttpMessage_base
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t set_protocol(exlib::string newVal);
    virtual result_t get_headers(obj_ptr<HttpHeaders_base>& retVal);
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
    virtual result_t appendHeader(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t appendHeader(exlib::string name, exlib::string value);
    virtual result_t setHeader(v8::Local<v8::Object> map);
    virtual result_t setHeader(exlib::string name, v8::Local<v8::Array> values);
    virtual result_t setHeader(exlib::string name, exlib::string value);
    virtual result_t removeHeader(exlib::string name);

public:
    // HttpResponse_base
    virtual result_t get_statusCode(int32_t& retVal);
    virtual result_t set_statusCode(int32_t newVal);
    virtual result_t get_statusMessage(exlib::string& retVal);
    virtual result_t set_statusMessage(exlib::string newVal);
    virtual result_t get_status(int32_t& retVal);
    virtual result_t set_status(int32_t newVal);
    virtual result_t get_ok(bool& retVal);
    virtual result_t writeHead(int32_t statusCode, exlib::string statusMessage, v8::Local<v8::Object> headers);
    virtual result_t writeHead(int32_t statusCode, v8::Local<v8::Object> headers);
    virtual result_t get_cookies(obj_ptr<NArray>& retVal);
    virtual result_t addCookie(HttpCookie_base* cookie);
    virtual result_t redirect(exlib::string url);
    virtual result_t redirect(int32_t statusCode, exlib::string url);

public:
    result_t sendHeader(Stream_base* stm, bool content_length, AsyncEvent* ac);
    result_t allHeader(exlib::string name, obj_ptr<NArray>& retVal)
    {
        return m_message->allHeader(name, retVal);
    }

    result_t appendHeader(NObject* map)
    {
        for (int32_t i = 0; i < (int32_t)map->m_values.size(); i++) {
            NObject::Value& v = map->m_values[i];

            if (!v.m_val.isUndefined()) {
                obj_ptr<NArray> list = NArray::getInstance(v.m_val.object());

                if (list) {
                    for (int32_t i = 0; i < (int32_t)list->m_array.size(); i++)
                        appendHeader(v.m_pos->first, list->m_array[i].string());
                } else
                    appendHeader(v.m_pos->first, v.m_val.string());
            }
        }

        return 0;
    }

    exlib::string prepareHeaders();

    result_t readHeader(Stream_base* stm, AsyncEvent* ac);
    result_t readBody(AsyncEvent* ac);

private:
    result_t readFrom(Stream_base* stm, AsyncEvent* ac, bool headerOnly);

public:
    obj_ptr<HttpMessage> m_message;
    int32_t m_statusCode;
    exlib::string m_statusMessage;
    obj_ptr<NArray> m_cookies;
};

} /* namespace fibjs */
