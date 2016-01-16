/*
 * JsonRpcHandler.h
 *
 *  Created on: Sep 3, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"

#ifndef JSONRPCHANDLER_H_
#define JSONRPCHANDLER_H_

namespace fibjs
{

class JsonRpcHandler: public Handler_base
{
    FIBER_FREE();

public:
    // object_base
    virtual result_t dispose()
    {
        m_handler.dispose();

        return 0;
    }

public:
    JsonRpcHandler(Handler_base *hdlr)
    {
        wrap()->SetHiddenValue(v8::String::NewFromUtf8(holder()->m_isolate, "handler"), hdlr->wrap());
        m_handler = hdlr;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal, AsyncEvent *ac);

private:
    naked_ptr<Handler_base> m_handler;
};

} /* namespace fibjs */
#endif /* JSONRPCHANDLER_H_ */
