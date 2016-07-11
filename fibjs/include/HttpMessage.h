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

namespace fibjs
{

#define HTTP_MAX_LINE   4096

class HttpMessage: public Message
{
public:
    HttpMessage(bool bResponse = false) :
        m_bResponse(bResponse), m_maxHeadersCount(128), m_maxUploadSize(67108864)
    {
        m_headers = new HttpCollection();
        clear();
    }

public:
    result_t get_protocol(qstring &retVal);
    result_t set_protocol(const char *newVal);
    result_t get_headers(obj_ptr<HttpCollection_base> &retVal);
    result_t get_keepAlive(bool &retVal);
    result_t set_keepAlive(bool newVal);
    result_t get_upgrade(bool& retVal);
    result_t set_upgrade(bool newVal);
    result_t get_maxHeadersCount(int32_t &retVal);
    result_t set_maxHeadersCount(int32_t newVal);
    result_t get_maxUploadSize(int32_t &retVal);
    result_t set_maxUploadSize(int32_t newVal);
    result_t hasHeader(const char *name, bool &retVal);
    result_t firstHeader(const char *name, Variant &retVal);
    result_t allHeader(const char *name, obj_ptr<List_base> &retVal);
    result_t addHeader(Map_base* map);
    result_t addHeader(const char *name, Variant value);
    result_t setHeader(Map_base* map);
    result_t setHeader(const char *name, Variant value);
    result_t removeHeader(const char *name);
    result_t get_stream(obj_ptr<Stream_base> &retVal);

    result_t clear();

public:
    result_t sendTo(Stream_base *stm, qstring &strCommand,
                    AsyncEvent *ac);
    result_t sendHeader(Stream_base *stm, qstring &strCommand,
                        AsyncEvent *ac);
    result_t readFrom(Stream_base *stm, AsyncEvent *ac);

public:
    void addHeader(const char *name, int32_t szName, const char *value,
                   int32_t szValue);
    result_t addHeader(qstring &strLine);
    size_t size();
    size_t getData(char *buf, size_t sz);

public:
    obj_ptr<Stream_base> m_stm;
    bool m_bResponse;
    qstring m_protocol;
    bool m_keepAlive;
    bool m_upgrade;
    int32_t m_maxHeadersCount;
    int32_t m_maxUploadSize;
    qstring m_origin;
    qstring m_encoding;
    obj_ptr<HttpCollection> m_headers;
};

} /* namespace fibjs */
#endif /* HTTPMESSAGE_H_ */
