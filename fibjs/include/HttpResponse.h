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
    // Options for sendTo/readFrom
    class Options : public obj_base {
    public:
        LOAD_OPTIONS(Options, (header_only)(content_length));

    public:
        std::optional<bool> header_only = false;
        std::optional<bool> content_length;
    };

    // Options for Web API compatible constructor: new Response(body, options)
    class ResponseOptions : public obj_base {
    public:
        LOAD_OPTIONS(ResponseOptions, (status)(statusText)(headers));

    public:
        std::optional<int32_t> status;
        std::optional<exlib::string> statusText;
        std::optional<std::variant<v8::Local<v8::Object>, obj_ptr<Headers_base>>> headers;
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
    virtual result_t get_body(obj_ptr<Stream_base>& retVal);
    virtual result_t set_body(Stream_base* newVal);
    virtual result_t get_bodyUsed(bool& retVal);
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac);
    virtual result_t text(exlib::string data, exlib::string& retVal, AsyncEvent* ac);
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac);
    virtual result_t json(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac);
    virtual result_t json(Variant& retVal, AsyncEvent* ac);
    virtual result_t pack(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac);
    virtual result_t pack(Variant& retVal, AsyncEvent* ac);
    virtual result_t blob(exlib::string type, obj_ptr<Blob_base>& retVal, AsyncEvent* ac);
    virtual result_t bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
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
    // HttpResponse_base
    virtual result_t get_statusCode(int32_t& retVal);
    virtual result_t set_statusCode(int32_t newVal);
    virtual result_t get_statusMessage(exlib::string& retVal);
    virtual result_t set_statusMessage(exlib::string newVal);
    virtual result_t get_statusText(exlib::string& retVal);
    virtual result_t set_statusText(exlib::string newVal);
    virtual result_t get_status(int32_t& retVal);
    virtual result_t set_status(int32_t newVal);
    virtual result_t get_ok(bool& retVal);
    virtual result_t writeHead(int32_t statusCode, exlib::string statusMessage, v8::Local<v8::Object> headers);
    virtual result_t writeHead(int32_t statusCode, v8::Local<v8::Object> headers);
    virtual result_t get_cookies(obj_ptr<NArray>& retVal);
    virtual result_t addCookie(HttpCookie_base* cookie);
    virtual result_t redirect(exlib::string url);
    virtual result_t redirect(int32_t statusCode, exlib::string url);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_redirected(bool& retVal);
    virtual result_t get_type(exlib::string& retVal);
    static result_t json(v8::Local<v8::Value> data, v8::Local<v8::Object> options, obj_ptr<HttpResponse_base>& retVal);
    static result_t redirect(exlib::string url, int32_t status, obj_ptr<HttpResponse_base>& retVal);
    static result_t error(obj_ptr<HttpResponse_base>& retVal);

public:
    result_t sendHeader(Stream_base* stm, bool content_length, AsyncEvent* ac);
    result_t allHeader(exlib::string name, obj_ptr<NArray>& retVal)
    {
        return m_message->allHeader(name, retVal);
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
    // Fetch API metadata
    exlib::string m_fetchUrl;
    bool m_redirected = false;
    exlib::string m_fetchType = "basic";
};

} /* namespace fibjs */
