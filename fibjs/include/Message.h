/*
 * Message.h
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "ifs/List.h"

#ifndef MESSAGE_H_
#define MESSAGE_H_

namespace fibjs
{

class Message
{
public:
	result_t get_value(std::string& retVal);
	result_t set_value(const char* newVal);
	result_t get_params(obj_ptr<List_base>& retVal);
	result_t set_params(List_base* newVal);

public:
	void clear()
	{
		m_value.clear();
		m_params.Release();
	}

	result_t set_value(std::string& newVal)
	{
		m_value = newVal;
		return 0;
	}

private:
	std::string m_value;
	obj_ptr<List_base> m_params;
};

} /* namespace fibjs */
#endif /* MESSAGE_H_ */
