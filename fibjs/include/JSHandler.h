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
	// Handler_base
	virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
			exlib::AsyncEvent* ac);

	result_t setHandler(v8::Handle<v8::Value> hdlr);

	static result_t New(v8::Handle<v8::Value> hdlr,
			obj_ptr<Handler_base>& retVal)
	{
		obj_ptr<JSHandler> r = new JSHandler();
		result_t hr = r->setHandler(hdlr);
		if (hr < 0)
			return hr;

		retVal = r;

		return 0;
	}

public:
	result_t js_invoke(object_base* v, obj_ptr<Handler_base>& retVal);

private:
	v8::Persistent<v8::Value> m_handler;
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
