/*
 * HttpMessage.h
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#pragma once

#include "Message.h"
#include "HttpCollection.h"
#include "Headers.h"
#include "ifs/BufferedStream.h"

namespace fibjs {

// Helper function to convert body value to SeekableStream
// Handles: null/undefined, string, Buffer, Blob, FormData, URLSearchParams, SeekableStream
// defaultFormUrlEncoded: true for HttpClient (historical), false for Response (Web API standard)
result_t body_to_stream(Isolate* isolate, v8::Local<v8::Value> body,
    obj_ptr<SeekableStream_base>& retVal, Headers_base* headers = NULL, bool defaultFormUrlEncoded = false);

class HttpMessage : public Message {
public:
    HttpMessage(bool bResponse = false)
        : m_bResponse(bResponse)
        , m_bNoBody(false)
        , m_maxHeadersCount(128)
        , m_maxHeaderSize(8192)
        , m_maxChunkSize(2)
        , m_maxBodySize(64)
        , m_contentLength(-1)
        , m_bChunked(false)
        , m_sent(false)
    {
        m_headers = new Headers();
        clear();
    }

public:
    // Message_base
    virtual result_t get_sent(bool& retVal);
    virtual result_t text(exlib::string data, exlib::string& retVal, AsyncEvent* ac);
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac);
    virtual result_t json(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac);
    virtual result_t json(Variant& retVal, AsyncEvent* ac);
    virtual result_t pack(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac);
    virtual result_t pack(Variant& retVal, AsyncEvent* ac);

public:
    result_t get_protocol(exlib::string& retVal);
    result_t set_protocol(exlib::string newVal);
    result_t get_headers(obj_ptr<Headers_base>& retVal);
    result_t get_keepAlive(bool& retVal);
    result_t set_keepAlive(bool newVal);
    result_t get_upgrade(bool& retVal);
    result_t set_upgrade(bool newVal);
    result_t get_maxHeadersCount(int32_t& retVal);
    result_t set_maxHeadersCount(int32_t newVal);
    result_t get_maxHeaderSize(int32_t& retVal);
    result_t set_maxHeaderSize(int32_t newVal);
    result_t get_maxChunkSize(int32_t& retVal);
    result_t set_maxChunkSize(int32_t newVal);
    result_t get_maxBodySize(int32_t& retVal);
    result_t set_maxBodySize(int32_t newVal);
    result_t get_socket(obj_ptr<Stream_base>& retVal);
    result_t hasHeader(exlib::string name, bool& retVal);
    result_t firstHeader(exlib::string name, exlib::string& retVal);
    result_t allHeader(exlib::string name, obj_ptr<NObject>& retVal);
    result_t appendHeader(v8::Local<v8::Object> map);
    result_t appendHeader(Headers_base* headers);
    result_t appendHeader(exlib::string name, v8::Local<v8::Array> values);
    result_t appendHeader(exlib::string name, exlib::string value);
    result_t setHeader(v8::Local<v8::Object> map);
    result_t setHeader(Headers_base* headers);
    result_t setHeader(exlib::string name, v8::Local<v8::Array> values);
    result_t setHeader(exlib::string name, exlib::string value);
    result_t removeHeader(exlib::string name);
    result_t get_stream(obj_ptr<Stream_base>& retVal);

    result_t clear();

    // Clone this message - must be implemented by concrete subclasses
    virtual result_t clone(obj_ptr<Message_base>& retVal);

    // Helper method to copy HttpMessage properties to another HttpMessage instance
    void copyTo(HttpMessage* target);

public:
    result_t send(Stream_base* stm, exlib::string& strCommand,
        AsyncEvent* ac);
    result_t sendHeader(Stream_base* stm, exlib::string& strCommand, bool content_length,
        AsyncEvent* ac);
    result_t readFrom(Stream_base* stm, AsyncEvent* ac);
    result_t readHeader(Stream_base* stm, AsyncEvent* ac);
    result_t readBody(AsyncEvent* ac);

public:
    void appendHeader(const char* name, int32_t szName, const char* value,
        int32_t szValue);
    result_t appendHeader(exlib::string& strLine);
    size_t getData(char* buf, size_t sz, bool content_length);

    result_t allHeader(exlib::string name, obj_ptr<NArray>& retVal)
    {
        return m_headers->all(name, retVal);
    }

public:
    obj_ptr<Stream_base> m_stm;
    obj_ptr<Stream_base> m_socket;
    bool m_bResponse;
    bool m_bNoBody;
    exlib::string m_protocol;
    bool m_keepAlive;
    bool m_upgrade;
    int32_t m_maxHeadersCount;
    int32_t m_maxHeaderSize;
    int32_t m_maxChunkSize;
    int32_t m_maxBodySize;
    exlib::string m_origin;
    exlib::string m_encoding;
    obj_ptr<Headers> m_headers;

    int64_t m_contentLength;
    bool m_bChunked;
    bool m_sent;
};

} /* namespace fibjs */
