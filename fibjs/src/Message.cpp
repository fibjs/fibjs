/*
 * Message.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "Message.h"
#include "List.h"
#include "MemoryStream.h"

namespace fibjs
{

result_t Message::get_value(std::string& retVal)
{
	retVal = m_value;
	return 0;
}

result_t Message::set_value(const char* newVal)
{
	m_value = newVal;
	return 0;
}

result_t Message::get_params(obj_ptr<List_base>& retVal)
{
	if(m_values == NULL)
		m_values = new values();

	if (m_values->m_params == NULL)
		m_values->m_params = new List();

	retVal = m_values->m_params;
	return 0;
}

result_t Message::set_params(List_base* newVal)
{
	if(m_values == NULL)
		m_values = new values();

	m_values->m_params = newVal;
	return 0;
}

result_t Message::get_result(Variant& retVal)
{
	if(m_values == NULL)
		m_values = new values();

	retVal = m_values->m_result;
	return 0;
}

result_t Message::set_result(Variant newVal)
{
	if(m_values == NULL)
		m_values = new values();

	m_values->m_result = newVal;
	return 0;
}

result_t Message::get_body(obj_ptr<SeekableStream_base>& retVal)
{
	if (m_body == NULL)
		m_body = new MemoryStream();

	retVal = m_body;
	return 0;
}

result_t Message::set_body(SeekableStream_base* newVal)
{
	m_body = newVal;
	return 0;
}

result_t Message::get_length(int64_t& retVal)
{
	if (m_body == NULL)
	{
		retVal = 0;
		return 0;
	}
	return m_body->size(retVal);
}

} /* namespace fibjs */
