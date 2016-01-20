/*
 * RpcTask.h
 *
 *  Created on: Jan 20, 2016
 *      Author: lion
 */

#include "ifs/RpcTask.h"
#include <map>

#ifndef RPCTASK_H_
#define RPCTASK_H_

namespace fibjs
{

class RpcTask: public RpcTask_base
{
public:
	RpcTask(const char* id) : m_id(id)
	{}

	RpcTask(RpcTask* t, const char* func)
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
	// RpcTask_base
	virtual result_t _function(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal);
	virtual result_t _named_getter(const char* property, obj_ptr<RpcTask_base>& retVal);
	virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);

private:
	std::map<std::string, obj_ptr<RpcTask_base> > m_funcs;
	std::string m_id;
	std::string m_func;
};

}

#endif /* RPCTASK_H_ */
