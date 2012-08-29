/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"

#ifndef JSHANDLER_H_
#define JSHANDLER_H_

namespace fibjs
{

class JSHandler: public Handler_base
{
public:
	JSHandler(v8::Handle<v8::Function> func)
	{
		m_func = v8::Persistent<v8::Function>::New(func);
	}

public:
	// Handler_base
	virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac);

public:
	result_t callFunction(object_base* v, obj_ptr<Handler_base>& retVal);

private:
	v8::Persistent<v8::Function> m_func;
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
