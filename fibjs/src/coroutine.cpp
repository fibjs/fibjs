/*
 * coroutine.cpp
 *
 *  Created on: Apr 9, 2012
 *      Author: lion
 */

#include "ifs/coroutine.h"
#include "ifs/os.h"
#include <vector>

namespace fibjs
{

static class null_fiber_data: public Fiber_base
{
EVENT_SUPPORT()
	;FIBER_FREE()
	;

public:
	null_fiber_data()
	{
		Ref();
	}

	result_t get_func(v8::Handle<v8::Function>& retVal)
	{
		return CALL_E_INVALID_CALL;
	}

	result_t join()
	{
		return 0;
	}

	result_t get_caller(obj_ptr<Fiber_base>& retVal)
	{
		return CALL_E_INVALID_CALL;
	}

	result_t onerror(v8::Handle<v8::Function> trigger)
	{
		return 0;
	}

	result_t onexit(v8::Handle<v8::Function> trigger)
	{
		return 0;
	}

} s_null;

class fiber_data: public Fiber_base
{
	EVENT_SUPPORT();
	FIBER_FREE();

public:
	fiber_data() :
		m_next(NULL)
	{
	}

	~fiber_data()
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

public:
	v8::Persistent<v8::Function> m_func;
	std::vector<v8::Persistent<v8::Value> > argv;
	fiber_data* m_next;
	exlib::Event m_quit;
	obj_ptr<Fiber_base> m_caller;
};

static exlib::Queue<fiber_data> g_jobs;
static exlib::IDLE_PROC s_oldIdle;
static int32_t s_cpus;
static int32_t s_fibers;
static int s_tlsCurrent;

#define FIBER_PER_CPU	3000
extern v8::Persistent<v8::Context> s_context;
static class _fiber_init
{
public:
	_fiber_init()
	{
		if (os_base::CPUs(s_cpus) < 0)
			s_cpus = 4;
		s_fibers = 0;

		exlib::Service* pService = exlib::Service::getFiberService();

		s_tlsCurrent = pService->tlsAlloc();
		s_oldIdle = pService->onIdle(onIdle);
	}

	static void* fiber_proc(void* p)
	{
		exlib::Service* pService = exlib::Service::getFiberService();

		v8::Locker locker(isolate);
		v8::Isolate::Scope isolate_scope(isolate);

		v8::HandleScope handle_scope;
		v8::Context::Scope context_scope(s_context);

		while (1)
		{
			fiber_data* fb;

			if ((fb = g_jobs.tryget()) == NULL)
			{
				v8::Unlocker unlocker(isolate);
				fb = g_jobs.get();
			}

			if (fb == NULL)
				break;

			pService->tlsPut(s_tlsCurrent, fb);

			v8::HandleScope handle_scope;

			size_t i;

			std::vector<v8::Handle<v8::Value> > argv;

			argv.resize(fb->argv.size());
			for (i = 0; i < fb->argv.size(); i++)
				argv[i] = fb->argv[i];

			v8::Handle<v8::Object> o = fb->wrap();

			v8::TryCatch try_catch;
			v8::Handle<v8::Value> retVal = fb->m_func->Call(o, (int) argv.size(), argv.data());
			if (try_catch.HasCaught())
			{
				v8::Handle<v8::Value> err = try_catch.Exception();
				fb->_trigger("error", &err, 1);
				ReportException(&try_catch, true);
				retVal = v8::Undefined();
			}

			fb->_trigger("exit", &retVal, 1);

			fb->m_quit.set();
			fb->dispose();
			fb->Unref();

			s_null.Ref();
			o->SetPointerInInternalField(0, &s_null);
		}

		return NULL;
	}

	static void onIdle()
	{
		if (!g_jobs.empty() && (s_fibers < s_cpus * FIBER_PER_CPU))
		{
			s_fibers++;
			exlib::Service::CreateFiber(fiber_proc)->Unref();
		}

		if (s_oldIdle)
			s_oldIdle();
	}
} s_fiber_init;

template<typename T>
result_t startJSFiber(v8::Handle<v8::Function> func, T& args, int nArgStart,
		int nArgCount, obj_ptr<Fiber_base>& retVal)
{
	v8::HandleScope handle_scope;

	fiber_data* fb = new fiber_data();
	int i;

	coroutine_base::current(fb->m_caller);

	fb->argv.resize(nArgCount - nArgStart);
	for (i = nArgStart; i < nArgCount; i++)
		fb->argv[i - nArgStart] = v8::Persistent<v8::Value>::New(args[i]);
	fb->m_func = v8::Persistent<v8::Function>::New(func);

	g_jobs.put(fb);
	fb->Ref();

	retVal = fb;

	return 0;
}

result_t startJSFiber(v8::Handle<v8::Function> func, const v8::Arguments& args,
		int nArgStart, obj_ptr<Fiber_base>& retVal)
{
	return startJSFiber(func, args, nArgStart, args.Length(), retVal);
}

result_t startJSFiber(v8::Handle<v8::Function> func,
		v8::Handle<v8::Value>* args, int argCount, obj_ptr<Fiber_base>& retVal)
{
	return startJSFiber(func, args, 0, argCount, retVal);
}

result_t coroutine_base::start(v8::Handle<v8::Function> func,
		const v8::Arguments& args, obj_ptr<Fiber_base>& retVal)
{
	return startJSFiber(func, args, 1, retVal);
}

result_t coroutine_base::current(obj_ptr<Fiber_base>& retVal)
{
	exlib::Service* pService = exlib::Service::getFiberService();
	Fiber_base* fb = (Fiber_base*) pService->tlsGet(s_tlsCurrent);

	if (!fb)
	{
		fb = new fiber_data();
		pService->tlsPut(s_tlsCurrent, fb);
		fb->Ref();
	}

	retVal = fb;

	return 0;
}

result_t coroutine_base::sleep(int32_t ms)
{
	if (ms > 0)
	{
		void* args[] =
		{ &ms };
		AsyncCall ac(s_acSleep, args);
	}
	else
	{
		v8::Unlocker unlocker(isolate);
		exlib::Fiber::yield();
	}

	return 0;
}

}
