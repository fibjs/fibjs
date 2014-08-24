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

namespace fibjs
{

class HttpResponse: public HttpResponse_base
{

public:
    HttpResponse() :
        m_message(true)
    {
        clear();
        extMemory(4096);
    }

public:
    // Message_base
    virtual result_t get_value(std::string &retVal);
    virtual result_t set_value(const char *newVal);
    virtual result_t get_params(obj_ptr<List_base> &retVal);
    virtual result_t set_params(List_base *newVal);
    virtual result_t get_result(Variant &retVal);
    virtual result_t set_result(Variant newVal);
    virtual result_t get_body(obj_ptr<SeekableStream_base> &retVal);
    virtual result_t set_body(SeekableStream_base *newVal);
    virtual result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
    virtual result_t get_length(int64_t &retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base *stm, exlib::AsyncEvent *ac);
    virtual result_t readFrom(BufferedStream_base *stm, exlib::AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_response(obj_ptr<Message_base> &retVal);

public:
    // HttpMessage_base
    virtual result_t get_protocol(std::string &retVal);
    virtual result_t set_protocol(const char *newVal);
    virtual result_t get_headers(obj_ptr<HttpCollection_base> &retVal);
    virtual result_t get_keepAlive(bool &retVal);
    virtual result_t set_keepAlive(bool newVal);
    virtual result_t get_maxHeadersCount(int32_t &retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxUploadSize(int32_t &retVal);
    virtual result_t set_maxUploadSize(int32_t newVal);
    virtual result_t hasHeader(const char *name, bool &retVal);
    virtual result_t firstHeader(const char *name, Variant &retVal);
    virtual result_t allHeader(const char *name, obj_ptr<List_base> &retVal);
    virtual result_t addHeader(v8::Local<v8::Object> map);
    virtual result_t addHeader(const char *name, Variant value);
    virtual result_t setHeader(v8::Local<v8::Object> map);
    virtual result_t setHeader(const char *name, Variant value);
    virtual result_t removeHeader(const char *name);

public:
    // HttpResponse_base
    virtual result_t get_status(int32_t &retVal);
    virtual result_t set_status(int32_t newVal);
    virtual result_t redirect(const char *url);

public:
    HttpMessage m_message;
    int32_t m_status;
};

} /* namespace fibjs */
#endif /* HTTPRESPONSE_H_ */
