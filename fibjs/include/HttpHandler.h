/*
 * HttpHandler.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/HttpHandler.h"

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
    // HttpHandler_base
    virtual result_t enableCrossOrigin(exlib::string allowHeaders);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_serverName(exlib::string& retVal);
    virtual result_t set_serverName(exlib::string newVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

private:
    obj_ptr<Handler_base> m_hdlr;

    bool m_crossDomain;
    exlib::string m_allowHeaders;
    int32_t m_maxHeadersCount;
    int32_t m_maxBodySize;
    exlib::string m_serverName;
};

} /* namespace fibjs */
