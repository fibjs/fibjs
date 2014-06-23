/*
 * NullHandler.h
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"

#ifndef NULLHANDLER_H_
#define NULLHANDLER_H_

namespace fibjs
{

class NullHandler: public Handler_base
{
    FIBER_FREE();

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal, exlib::AsyncEvent *ac);
};

} /* namespace fibjs */
#endif /* NULLHANDLER_H_ */
