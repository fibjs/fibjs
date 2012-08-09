/*
 * HttpRequest.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "HttpRequest.h"

namespace fibjs
{

result_t HttpRequest_base::_new(obj_ptr<HttpRequest_base>& retVal)
{
	retVal = new HttpRequest();
	return 0;
}

result_t HttpRequest::get_protocol(int32_t& retVal)
{
	return 0;
}

result_t HttpRequest::set_protocol(int32_t newVal)
{
	return 0;
}

result_t HttpRequest::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
	retVal = m_headers;
	return 0;
}

result_t HttpRequest::get_body(obj_ptr<Stream_base>& retVal)
{
	return 0;
}

result_t HttpRequest::set_body(obj_ptr<Stream_base>& newVal)
{
	return 0;
}

result_t HttpRequest::get_contentType(std::string& retVal)
{
	return 0;
}

result_t HttpRequest::set_contentType(const char* newVal)
{
	return 0;
}

result_t HttpRequest::get_contentLength(int32_t& retVal)
{
	return 0;
}

result_t HttpRequest::set_contentLength(int32_t newVal)
{
	return 0;
}

result_t HttpRequest::get_keepAlive(bool& retVal)
{
	return 0;
}

result_t HttpRequest::set_keepAlive(bool newVal)
{
	return 0;
}

result_t HttpRequest::clear()
{
	return 0;
}

result_t HttpRequest::send(obj_ptr<Stream_base>& stm)
{
	return 0;
}

result_t HttpRequest::asyncSend(obj_ptr<Stream_base>& stm)
{
	return 0;
}

result_t HttpRequest::onsend(v8::Handle<v8::Function> func)
{
	return 0;
}

result_t HttpRequest::read(obj_ptr<BufferedStream_base>& stm)
{
	return 0;
}

result_t HttpRequest::asyncRead(obj_ptr<BufferedStream_base>& stm)
{
	return 0;
}

result_t HttpRequest::onread(v8::Handle<v8::Function> func)
{
	return 0;
}

result_t HttpRequest::get_method(std::string& retVal)
{
	return 0;
}

result_t HttpRequest::set_method(const char* newVal)
{
	return 0;
}

result_t HttpRequest::get_address(std::string& retVal)
{
	return 0;
}

result_t HttpRequest::set_address(const char* newVal)
{
	return 0;
}

result_t HttpRequest::get_queryString(std::string& retVal)
{
	return 0;
}

result_t HttpRequest::set_queryString(const char* newVal)
{
	return 0;
}

result_t HttpRequest::get_cookies(obj_ptr<HttpCollection_base>& retVal)
{
	return 0;
}

result_t HttpRequest::get_form(obj_ptr<HttpCollection_base>& retVal)
{
	return 0;
}

result_t HttpRequest::get_query(obj_ptr<HttpCollection_base>& retVal)
{
	return 0;
}

} /* namespace fibjs */
