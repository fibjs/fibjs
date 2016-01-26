/*
 * RpcTask.h
 *
 *  Created on: Jan 20, 2016
 *      Author: lion
 */

#include "ifs/RpcTask.h"
#include <map>
#include <vector>

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
		AsyncTask(RpcTask* task) :
			AsyncCall(NULL), m_task(task)
		{}

	public:
		virtual v8::Local<v8::Value> get_result() = 0;
		virtual void set_result(v8::Local<v8::Value> newVal) = 0;
		virtual void get_param(v8::Isolate* isolate, std::vector<v8::Local<v8::Value> >& retVal) = 0;
		virtual void set_param(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
		virtual std::string get_error() = 0;
		virtual void set_error(std::string newVal) = 0;

	public:
		obj_ptr<RpcTask> m_task;
	};

public:
	std::map<std::string, obj_ptr<RpcTask_base> > m_funcs;
	std::string m_id;
	std::string m_method_path;
};

}

#endif /* RPCTASK_H_ */
