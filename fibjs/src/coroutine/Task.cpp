/*
 * Task.cpp
 *
 *  Created on: Apr 30, 2012
 *      Author: lion
 */

#include "Task.h"

namespace fibjs
{

result_t Task_base::_new(const char* id, obj_ptr<Task_base>& retVal,
                         v8::Local<v8::Object> This)
{
	retVal = new Task(id);
	return 0;
}

result_t Task::_named_getter(const char* property, obj_ptr<Task_base>& retVal)
{
	std::map<std::string, obj_ptr<Task_base> >::iterator it = m_funcs.find(property);

	if (it != m_funcs.end())
		retVal = it->second;
	else
	{
		retVal = new Task(this, property);
		m_funcs.insert(std::pair<std::string, obj_ptr<Task_base> >(property, retVal));
	}

	return 0;
}

result_t Task::_named_enumerator(v8::Local<v8::Array>& retVal)
{
	int32_t i = 0;
	Isolate* isolate = holder();

	retVal = v8::Array::New(isolate->m_isolate, (int32_t)m_funcs.size());
	std::map<std::string, obj_ptr<Task_base> >::iterator iter;

	for (iter = m_funcs.begin(); iter != m_funcs.end(); iter++)
		retVal->Set(i++,
		            v8::String::NewFromUtf8(isolate->m_isolate, iter->first.c_str(),
		                                    v8::String::kNormalString,
		                                    (int32_t) iter->first.length()));

	return 0;
}

result_t Task::toString(std::string &retVal)
{
	retVal = m_id;
	if (!m_func.empty())
	{
		retVal.append(": ", 2);
		retVal.append(m_func);

	}
	return 0;
}

}
