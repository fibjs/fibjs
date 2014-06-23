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
    JsonRpcHandler(Handler_base *hdlr) :
        m_hdlr(hdlr)
    {
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal, exlib::AsyncEvent *ac);

private:
    obj_ptr<Handler_base> m_hdlr;
};

} /* namespace fibjs */
#endif /* JSONRPCHANDLER_H_ */
