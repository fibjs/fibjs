/*
 * RpcTask.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: lion
 */

#include "ifs/rpc.h"
#include "ifs/json.h"
#include "RpcTask.h"
#include "JsonRpcHandler.h"
#include "JSHandler.h"
#include "SandBox.h"
#include "Fiber.h"
#include "Url.h"
#include "vector"

namespace fibjs
{

static exlib::atomic s_vms;
static exlib::atomic s_idles;
static exlib::Queue<RpcTask::AsyncTask> s_acTask;

static void init_task_fiber(Isolate* isolate);

void nextMethod(std::string &method_path, std::string &method)
{
	const char *p, *p1;

	method.clear();
	p = p1 = method_path.c_str();
	while (true)
	{
		while (*p && *p != '.')
			p++;
		if (p != p1)
			break;
		if (!*p)
			return;
		p++;
		p1 = p;
	}

	method.assign(p1, (int32_t) (p - p1));
	method_path = *p ? p + 1 : "";
}

static void task_fiber(Isolate* isolate)
{
	RpcTask::AsyncTask *p;
	JSFiber::scope s;

	s_idles.inc();

	p = s_acTask.tryget();
	if (!p)
	{
		v8::Unlocker unlocker(isolate->m_isolate);
		p = s_acTask.get();
	}

	if (s_idles.dec() == 0 && s_vms > 0)
	{
		if (s_vms.dec() < 0)
			s_vms.inc();
		else
		{
			s_idles.inc();
			Isolate* new_isolate = new Isolate(NULL);
			syncCall(new_isolate, init_task_fiber, new_isolate);
		}
	}

	syncCall(isolate, task_fiber, isolate);

	v8::Local<v8::Value> v, v1;
	v8::Local<v8::Array> param;
	result_t hr;

	json_base::decode(p->m_param.c_str(), v);
	param = v8::Local<v8::Array>::Cast(v);

	int32_t len = param->Length();
	int32_t i;

	TryCatch try_catch;
	hr = isolate->m_topSandbox->require(p->m_task->m_id.c_str(), v);
	if (hr < 0)
	{
		p->m_error = GetException(try_catch, hr).c_str();
		p->post(hr);
		return;
	}

	std::string method_path = p->m_task->m_method_path;
	std::string method;
	v8::Local<v8::Object> o;

	v1 = v;
	while (true)
	{
		nextMethod(method_path, method);
		if (method.empty())
			break;

		if (!v->IsObject())
		{
			hr = CHECK_ERROR(Runtime::setError("RpcTask: method \"" + method + "\" not found."));;
			break;
		}

		o = v8::Local<v8::Object>::Cast(v);
		v1 = v;
		v = o->Get(isolate->NewFromUtf8(method));
		if (v.IsEmpty() || v->IsUndefined())
		{
			hr = CHECK_ERROR(Runtime::setError("RpcTask: method \"" + method + "\" not found."));;
			break;
		}
	}

	if (hr < 0)
	{
		p->m_error = GetException(try_catch, hr).c_str();
		p->post(hr);
		return;
	}

	if (v->IsFunction())
	{
		std::vector<v8::Local<v8::Value> > argv;

		argv.resize(len);
		for (i = 0; i < len; i++)
			argv[i] = v8::Local<v8::Value>::New(isolate->m_isolate, param->Get(i));

		v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(v);
		v = func->Call(v1, (int32_t) argv.size(), argv.data());
		if (v.IsEmpty())
		{
			p->m_error = GetException(try_catch, CALL_E_JAVASCRIPT).c_str();
			p->post(CALL_E_JAVASCRIPT);
			return;
		}

		json_base::encode(v, p->m_result);
	}

	p->post(0);
}

static void init_task_fiber(Isolate* isolate)
{
	s_idles.dec();
	task_fiber(isolate);
}

void init_Task(int32_t vms)
{
	s_vms = vms;
}

result_t RpcTask::_function(const v8::FunctionCallbackInfo<v8::Value>& args,
                            v8::Local<v8::Value>& retVal)
{
	static bool s_init = false;

	if (!s_init)
	{
		s_init = true;

		s_vms.dec();
		s_idles.inc();
		Isolate* new_isolate = new Isolate(NULL);
		syncCall(new_isolate, init_task_fiber, new_isolate);
	}

	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Array> array = v8::Array::New(isolate);
	int32_t i;

	for (i = 0; i < args.Length(); i ++)
		array->Set(i, args[i]);

	std::string param;
	result_t hr = json_base::encode(array, param);
	if (hr < 0)
		return hr;

	AsyncTask at(this, param);

	s_acTask.put(&at);
	hr = at.wait();
	if (hr < 0)
		return CHECK_ERROR(Runtime::setError(at.m_error));

	json_base::decode(at.m_result.c_str(), retVal);

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
	if (!m_method_path.empty())
	{
		retVal.append(": ", 2);
		retVal.append(m_method_path);

	}
	return 0;
}

result_t rpc_base::open(const char* id, obj_ptr<RpcTask_base>& retVal)
{
	Url _url;

	_url.parse(id);

	if (!_url.m_protocol.empty())
		return CHECK_ERROR(CALL_E_INVALIDARG);

	std::string& path = _url.m_pathname;
	if (path[0] == '.' && (isPathSlash(path[1]) || (path[1] == '.' && isPathSlash(path[2]))))
		return CHECK_ERROR(CALL_E_INVALIDARG);

	retVal = new RpcTask(path);
	return 0;
}

result_t rpc_base::json(v8::Local<v8::Value> hdlr,
                        obj_ptr<Handler_base> &retVal)
{
	obj_ptr<Handler_base> hdlr1;
	result_t hr = JSHandler::New(hdlr, hdlr1);
	if (hr < 0)
		return hr;

	retVal = new JsonRpcHandler(hdlr1);
	return 0;
}

}
