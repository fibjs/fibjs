/*
 * JSHandler.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "JSHandler.h"

namespace fibjs
{

result_t JSHandler_base::_new(v8::Handle<v8::Function> fun,
		obj_ptr<JSHandler_base>& retVal)
{
	return 0;
}

JSHandler::JSHandler()
{
	// TODO Auto-generated constructor stub

}

JSHandler::~JSHandler()
{
	// TODO Auto-generated destructor stub
}

result_t JSHandler::invoke(obj_ptr<object_base>& v,
		obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac)
{
	return 0;
}

} /* namespace fibjs */
