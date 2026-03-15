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
#include "Url.h"
#include "AbortController.h"

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
    // HttpRequest_base
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal);
    virtual result_t get_method(exlib::string& retVal);
    virtual result_t set_method(exlib::string newVal);
    virtual result_t get_address(exlib::string& retVal);
    virtual result_t set_address(exlib::string newVal);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_href(exlib::string& retVal);
    virtual result_t get_queryString(exlib::string& retVal);
    virtual result_t set_queryString(exlib::string newVal);
    virtual result_t get_cookies(obj_ptr<HttpCollection_base>& retVal);
    virtual result_t get_form(obj_ptr<FormData_base>& retVal);
    virtual result_t get_query(obj_ptr<URLSearchParams_base>& retVal);

public:
    // Options holds all parsed fetch/request parameters.
    // Inherits obj_base so it can be stored as a single slot in AsyncEvent::m_ctx.
    class Options : public obj_base {
    public:
        exlib::string method;
        obj_ptr<Url> u;
        obj_ptr<Headers_base> headers;
        obj_ptr<SeekableStream_base> body;
        bool keepAlive = true;
        bool has_keepAlive = false;
        exlib::string redirect = "follow"; // "follow" | "manual" | "error"
        bool redirected = false;
        bool streaming = true;
        obj_ptr<AbortSignal_base> signal;

        // Cast to concrete AbortSignal for internal C++ use (addAbortCallback / clearAbort).
        // Safe because AbortSignal is the only concrete implementation.
        AbortSignal* abort_signal() const { return static_cast<AbortSignal*>((AbortSignal_base*)signal); }

        // Parse method/headers/body/keepAlive from a v8 opts object.
        // urlEncoded_default: true  → string body gets application/x-www-form-urlencoded
        //                     false → string body gets text/plain;charset=UTF-8
        // strict: true → reject GET/HEAD requests with a body (Fetch API)
        result_t from_opts(exlib::string default_method, v8::Local<v8::Object> opts,
            bool urlEncoded_default, bool strict = false);

        // Fill u (URL), body fallback, and merged headers from an existing request.
        // Must be called after from_opts so that opts-supplied values take precedence.
        result_t apply_from_request(HttpRequest_base* req);

        // Resolve u from a base URL string + URL-override fields in opts.
        result_t resolve_url(exlib::string url, v8::Local<v8::Object> opts);

        // Apply a fallback keepAlive value when the caller did not specify one.
        void apply_keepalive_default(bool default_val)
        {
            if (!has_keepAlive)
                keepAlive = default_val;
        }
    };

public:
    // Apply parsed Options to this request (method, headers, body, keepAlive).
    void set_options(const Options& o);

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
