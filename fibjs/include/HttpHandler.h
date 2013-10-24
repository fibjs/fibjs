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
	HttpHandler(Handler_base* hdlr) :
			m_hdlr(hdlr), m_crossDomain(false), m_forceGZIP(false), m_maxHeadersCount(
					128), m_maxUploadSize(64)
	{
	}

public:
	// Handler_base
	virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
			exlib::AsyncEvent* ac);

public:
	// HttpHandler_base
	virtual result_t get_crossDomain(bool& retVal);
	virtual result_t set_crossDomain(bool newVal);
	virtual result_t get_forceGZIP(bool& retVal);
	virtual result_t set_forceGZIP(bool newVal);
	virtual result_t get_maxHeadersCount(int32_t& retVal);
	virtual result_t set_maxHeadersCount(int32_t newVal);
	virtual result_t get_maxUploadSize(int32_t& retVal);
	virtual result_t set_maxUploadSize(int32_t newVal);

private:
	obj_ptr<Handler_base> m_hdlr;
	bool m_crossDomain;
	bool m_forceGZIP;
	int32_t m_maxHeadersCount;
	int32_t m_maxUploadSize;
};

} /* namespace fibjs */
#endif /* HTTPHANDLER_H_ */
