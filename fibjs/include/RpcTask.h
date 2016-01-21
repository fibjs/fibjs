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
	FIBER_FREE();

public:
	RpcTask(std::string id) : m_id(id)
	{}

	RpcTask(RpcTask* t, const char* method)
	{
		m_id = t->m_id;
		if (!t->m_method_path.empty())
		{
			m_method_path = t->m_method_path;
			m_method_path.append(1, '.');
		}

		m_method_path.append(method);
	}

public:
	// Object_base
	virtual result_t toString(std::string &retVal);

public:
	// RpcTask_base
	virtual result_t _function(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal);
	virtual result_t _named_getter(const char* property, obj_ptr<RpcTask_base>& retVal);
	virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);

public:
	class AsyncTask : public AsyncCall
	{
	public:
		AsyncTask(RpcTask* task, std::string param) :
			AsyncCall(NULL), m_task(task), m_param(param)
		{}

	public:
		obj_ptr<RpcTask> m_task;
		std::string m_param;
		std::string m_result;
		std::string m_error;
	};

public:
	std::map<std::string, obj_ptr<RpcTask_base> > m_funcs;
	std::string m_id;
	std::string m_method_path;
};

}

#endif /* RPCTASK_H_ */
