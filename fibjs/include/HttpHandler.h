/*
 * HttpHandler.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/HttpHandler.h"

#ifndef HTTPHANDLER_H_
#define HTTPHANDLER_H_

namespace fibjs
{

class HttpHandler: public HttpHandler_base
{
public:
	HttpHandler(obj_ptr<Handler_base>& hdlr) :
			m_hdlr(hdlr)
	{
	}

public:
	// Handler_base
	virtual result_t invoke(obj_ptr<object_base>& v,
			obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac);

private:
	obj_ptr<Handler_base> m_hdlr;
};

} /* namespace fibjs */
#endif /* HTTPHANDLER_H_ */
