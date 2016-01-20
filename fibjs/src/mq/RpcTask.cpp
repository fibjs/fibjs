/*
 * RpcTask.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: lion
 */

#include "RpcTask.h"

namespace fibjs
{

result_t RpcTask_base::_new(const char* id, obj_ptr<RpcTask_base>& retVal,
                            v8::Local<v8::Object> This)
{
	retVal = new RpcTask(id);
	return 0;
}

result_t RpcTask::_named_getter(const char* property, obj_ptr<RpcTask_base>& retVal)
{
	std::map<std::string, obj_ptr<RpcTask_base> >::iterator it = m_funcs.find(property);

	if (it != m_funcs.end())
		retVal = it->second;
	else
	{
		retVal = new RpcTask(this, property);
		m_funcs.insert(std::pair<std::string, obj_ptr<RpcTask_base> >(property, retVal));
	}

	return 0;
}

result_t RpcTask::_named_enumerator(v8::Local<v8::Array>& retVal)
{
	int32_t i = 0;
	Isolate* isolate = holder();

	retVal = v8::Array::New(isolate->m_isolate, (int32_t)m_funcs.size());
	std::map<std::string, obj_ptr<RpcTask_base> >::iterator iter;

	for (iter = m_funcs.begin(); iter != m_funcs.end(); iter++)
		retVal->Set(i++, isolate->NewFromUtf8(iter->first));

	return 0;
}

result_t RpcTask::toString(std::string &retVal)
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
