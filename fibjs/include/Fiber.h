/*
 * Fiber.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "ifs/coroutine.h"
#include "ifs/Fiber.h"
#include "Runtime.h"
#include <vector>

#ifndef FIBER_H_
#define FIBER_H_

namespace fibjs
{

extern int g_tlsCurrent;

class FiberBase: public Fiber_base, asyncEvent
{
EVENT_SUPPORT()
	;FIBER_FREE()
	;

public:
	FiberBase()
	{
	}

	~FiberBase()
	{
	}

	result_t get_func(v8::Handle<v8::Function>& retVal)
	{
		return CALL_E_INVALID_CALL;
	}

	result_t join()
	{
		v8::Unlocker unlocker(isolate);
		m_quit.wait();

		return 0;
	}

	result_t get_caller(obj_ptr<Fiber_base>& retVal)
	{
		if (m_caller == NULL)
			return CALL_RETURN_NULL;

		retVal = m_caller;
		return 0;
	}

	result_t onerror(v8::Handle<v8::Function> trigger)
	{
		return on("error", trigger);
	}

	result_t onexit(v8::Handle<v8::Function> trigger)
	{
		return on("exit", trigger);
	}

public:
	static void* fiber_proc(void* p);
	void start();

	void exit()
	{
		m_quit.set();
		dispose();
		Unref();
	}

	Runtime& runtime()
	{
		return m_rt;
	}

private:
	exlib::Event m_quit;
	Runtime m_rt;
	obj_ptr<Fiber_base> m_caller;
};

class JSFiber: public FiberBase
{
EVENT_SUPPORT()
	;FIBER_FREE()
	;

public:
	JSFiber() :
			m_error(false)
	{
	}

	~JSFiber()
	{
		size_t i;

		for (i = 0; i < m_argv.size(); i++)
			m_argv[i].Dispose();

		m_func.Dispose();
		m_result.Dispose();
	}

	result_t get_func(v8::Handle<v8::Function>& retVal)
	{
		retVal = m_func;
		return 0;
	}

	virtual void js_callback();

	template<typename T>
	void New(v8::Handle<v8::Function> func, T& args, int nArgStart,
			int nArgCount)
	{
		v8::HandleScope handle_scope;
		int i;

		m_argv.resize(nArgCount - nArgStart);
		for (i = nArgStart; i < nArgCount; i++)
			m_argv[i - nArgStart] = v8::Persistent<v8::Value>::New(args[i]);
		m_func = v8::Persistent<v8::Function>::New(func);

		start();
	}

	template<typename T>
	static result_t New(v8::Handle<v8::Function> func,
			const v8::Arguments& args, int nArgStart, obj_ptr<T>& retVal)
	{
		obj_ptr<JSFiber> fb = new JSFiber();
		fb->New(func, args, nArgStart, args.Length());
		retVal = fb;

		return 0;
	}

	template<typename T>
	static result_t New(v8::Handle<v8::Function> func,
			v8::Handle<v8::Value>* args, int argCount, obj_ptr<T>& retVal)
	{
		obj_ptr<JSFiber> fb = new JSFiber();
		fb->New(func, args, 0, argCount);
		retVal = fb;

		return 0;
	}

	static void callFunction(v8::Handle<v8::Function> func,
			v8::Handle<v8::Value>* args, int argCount,
			v8::Handle<v8::Value>& retVal);

	result_t get_result(v8::Handle<v8::Value>& retVal)
	{
		if (m_result.IsEmpty())
			return CALL_RETURN_NULL;

		retVal = m_result;
		return 0;
	}

	bool isError()
	{
		return m_error;
	}

private:
	void callFunction(v8::Handle<v8::Value>& retVal);

private:
	v8::Persistent<v8::Function> m_func;
	std::vector<v8::Persistent<v8::Value> > m_argv;
	v8::Persistent<v8::Value> m_result;
	bool m_error;
};

} /* namespace fibjs */
#endif /* FIBER_H_ */
