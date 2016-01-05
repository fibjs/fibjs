/*
 * WebSocketHandler.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "ifs/WebSocketHandler.h"
#include "Stats.h"

#ifndef WEBSOCKETHANDLER_H_
#define WEBSOCKETHANDLER_H_

namespace fibjs
{

class WebSocketHandler: public WebSocketHandler_base
{
    FIBER_FREE();

public:
    WebSocketHandler();

public:
    // object_base
    virtual result_t dispose()
    {
        return 0;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            AsyncEvent *ac);

public:
    // HandlerEx_base
    virtual result_t onerror(v8::Local<v8::Object> hdlrs);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);
    virtual result_t get_stats(obj_ptr<Stats_base>& retVal);

public:
    // WebSocketHandler_base
    virtual result_t get_maxSize(int32_t &retVal);
    virtual result_t set_maxSize(int32_t newVal);

public:
    obj_ptr<Stream_base> m_stm;
    obj_ptr<Stats> m_stats;

private:
    naked_ptr<Handler_base> m_hdlr;
    naked_ptr<Handler_base> m_err_hdlr;
    int32_t m_maxSize;
};

} /* namespace fibjs */
#endif /* WEBSOCKETHANDLER_H_ */
