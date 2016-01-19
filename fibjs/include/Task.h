/*
 * Task.h
 *
 *  Created on: Jan 20, 2016
 *      Author: lion
 */

#include "ifs/Task.h"
#include <map>

#ifndef TASK_H_
#define TASK_H_

namespace fibjs
{

class Task: public Task_base
{
public:
	Task(const char* id) : m_id(id)
	{}

	Task(Task* t, const char* func)
	{
		m_id = t->m_id;
		if (!t->m_func.empty())
		{
			m_func = t->m_func;
			m_func.append(1, '.');
		}

		m_func.append(func);
	}

public:
	// Object_base
	virtual result_t toString(std::string &retVal);

public:
	// Task_base
	virtual result_t _named_getter(const char* property, obj_ptr<Task_base>& retVal);
	virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);

private:
	std::map<std::string, obj_ptr<Task_base> > m_funcs;
	std::string m_id;
	std::string m_func;
};

}

#endif /* TASK_H_ */
