/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "ifs/JSHandler.h"

#ifndef JSHANDLER_H_
#define JSHANDLER_H_

namespace fibjs
{

class JSHandler: public JSHandler_base
{
public:
	JSHandler();
	virtual ~JSHandler();

public:
	// Handler_base
	virtual result_t invoke(obj_ptr<object_base>& v, obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac);

private:
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
