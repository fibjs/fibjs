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

class Fiber: public fibjs::Fiber_base
{
	EVENT_SUPPORT();
	FIBER_FREE();

public:
	Fiber() :
	m_next(NULL)
	{
	}

	~Fiber()
	{
		size_t i;

		for (i = 0; i < argv.size(); i++)
			argv[i].Dispose();

		m_func.Dispose();
	}

	result_t get_func(v8::Handle<v8::Function>& retVal)
	{
		retVal = m_func;
		return 0;
	}

	result_t join()
	{
		v8::Unlocker unlocker(isolate);
		m_quit.wait();

		return 0;
	}

	result_t get_caller(obj_ptr<Fiber_base>& retVal)
	{
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

	template<typename T>
	static result_t startJSFiber(v8::Handle<v8::Function> func, T& args,
			int nArgStart, int nArgCount, obj_ptr<Fiber_base>& retVal)
	{
		v8::HandleScope handle_scope;

		Fiber* fb = new Fiber();
		int i;

		coroutine_base::current(fb->m_caller);

		fb->argv.resize(nArgCount - nArgStart);
		for (i = nArgStart; i < nArgCount; i++)
			fb->argv[i - nArgStart] = v8::Persistent<v8::Value>::New(args[i]);
		fb->m_func = v8::Persistent<v8::Function>::New(func);

		fb->start();

		retVal = fb;

		return 0;
	}

	static result_t startJSFiber(v8::Handle<v8::Function> func, const v8::Arguments& args,
			int nArgStart, obj_ptr<Fiber_base>& retVal)
	{
		return startJSFiber(func, args, nArgStart, args.Length(), retVal);
	}

	static result_t startJSFiber(v8::Handle<v8::Function> func,
			v8::Handle<v8::Value>* args, int argCount, obj_ptr<Fiber_base>& retVal)
	{
		return startJSFiber(func, args, 0, argCount, retVal);
	}

	static void* fiber_proc(void* p);
	void start();

	Runtime& runtime()
	{
		return m_rt;
	}

public:
	Fiber* m_next;

private:
	v8::Persistent<v8::Function> m_func;
	std::vector<v8::Persistent<v8::Value> > argv;
	exlib::Event m_quit;
	obj_ptr<Fiber_base> m_caller;
	Runtime m_rt;
};

} /* namespace fibjs */
#endif /* FIBER_H_ */
