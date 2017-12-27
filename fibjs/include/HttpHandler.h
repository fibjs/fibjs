/*
 * HttpHandler.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/HttpHandler.h"
#include "Stats.h"

#ifndef HTTPHANDLER_H_
#define HTTPHANDLER_H_

namespace fibjs {

class HttpHandler : public HttpHandler_base {
    FIBER_FREE();

public:
    HttpHandler();

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    // HandlerEx_base
    virtual result_t onerror(v8::Local<v8::Object> hdlrs);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);
    virtual result_t get_stats(obj_ptr<Stats_base>& retVal);

public:
    // HttpHandler_base
    virtual result_t enableCrossOrigin(exlib::string allowHeaders);
    virtual result_t get_forceGZIP(bool& retVal);
    virtual result_t set_forceGZIP(bool newVal);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_serverName(exlib::string& retVal);
    virtual result_t set_serverName(exlib::string newVal);

public:
    obj_ptr<Stats> m_stats;

private:
    obj_ptr<Handler_base> m_hdlr;
    obj_ptr<Handler_base> m_err_hdlrs[3];

    bool m_crossDomain;
    exlib::string m_allowHeaders;
    bool m_forceGZIP;
    int32_t m_maxHeadersCount;
    int32_t m_maxBodySize;
    exlib::string m_serverName;
};

} /* namespace fibjs */
#endif /* HTTPHANDLER_H_ */
