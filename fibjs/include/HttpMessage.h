/*
 * HttpMessage.h
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "Message.h"
#include "HttpCollection.h"
#include "ifs/BufferedStream.h"

#ifndef HTTPMESSAGE_H_
#define HTTPMESSAGE_H_

namespace fibjs {

#define HTTP_MAX_LINE 4096

class HttpMessage : public Message {
public:
    HttpMessage(bool bResponse = false)
        : m_bResponse(bResponse)
        , m_maxHeadersCount(128)
        , m_maxBodySize(64)
    {
        m_headers = new HttpCollection();
        clear();
    }

public:
    // Message_base
    virtual result_t json(v8::Local<v8::Value> data, v8::Local<v8::Value>& retVal);
    virtual result_t json(v8::Local<v8::Value>& retVal);

public:
    result_t get_protocol(exlib::string& retVal);
    result_t set_protocol(exlib::string newVal);
    result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
    result_t get_keepAlive(bool& retVal);
    result_t set_keepAlive(bool newVal);
    result_t get_upgrade(bool& retVal);
    result_t set_upgrade(bool newVal);
    result_t get_maxHeadersCount(int32_t& retVal);
    result_t set_maxHeadersCount(int32_t newVal);
    result_t get_maxBodySize(int32_t& retVal);
    result_t set_maxBodySize(int32_t newVal);
    result_t get_socket(obj_ptr<Stream_base>& retVal);
    result_t hasHeader(exlib::string name, bool& retVal);
    result_t firstHeader(exlib::string name, exlib::string& retVal);
    result_t allHeader(exlib::string name, obj_ptr<NArray>& retVal);
    result_t addHeader(v8::Local<v8::Object> map);
    result_t addHeader(exlib::string name, v8::Local<v8::Array> values);
    result_t addHeader(exlib::string name, exlib::string value);
    result_t setHeader(v8::Local<v8::Object> map);
    result_t setHeader(exlib::string name, v8::Local<v8::Array> values);
    result_t setHeader(exlib::string name, exlib::string value);
    result_t removeHeader(exlib::string name);
    result_t get_stream(obj_ptr<Stream_base>& retVal);

    result_t clear();

public:
    result_t sendTo(Stream_base* stm, exlib::string& strCommand,
        AsyncEvent* ac);
    result_t sendHeader(Stream_base* stm, exlib::string& strCommand,
        AsyncEvent* ac);
    result_t readFrom(Stream_base* stm, AsyncEvent* ac);

public:
    void addHeader(const char* name, int32_t szName, const char* value,
        int32_t szValue);
    result_t addHeader(exlib::string& strLine);
    size_t size();
    size_t getData(char* buf, size_t sz);

public:
    obj_ptr<Stream_base> m_stm;
    obj_ptr<Stream_base> m_socket;
    bool m_bResponse;
    exlib::string m_protocol;
    bool m_keepAlive;
    bool m_upgrade;
    int32_t m_maxHeadersCount;
    int32_t m_maxBodySize;
    exlib::string m_origin;
    exlib::string m_encoding;
    obj_ptr<HttpCollection> m_headers;
};

} /* namespace fibjs */
#endif /* HTTPMESSAGE_H_ */
