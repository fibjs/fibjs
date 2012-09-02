/*
 * Message.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "Message.h"
#include "List.h"

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
	if (m_params == NULL)
		m_params = new List();

	retVal = m_params;
	return 0;
}

result_t Message::set_params(List_base* newVal)
{
	m_params = newVal;
	return 0;
}

result_t Message::get_result(Variant& retVal)
{
	retVal = m_result;
	return 0;
}

result_t Message::set_result(Variant newVal)
{
	m_result = newVal;
	return 0;
}

} /* namespace fibjs */
