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
}s_null;

class fiber_data: public Fiber_base
{
EVENT_SUPPORT()
	;
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

public:
	v8::Persistent<v8::Function> m_func;
	std::vector<v8::Persistent<v8::Value> > argv;
	fiber_data* m_next;
	exlib::Event m_quit;
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

			v8::Handle<v8::Function> func;
			std::vector<v8::Handle<v8::Value> > argv;

			func = fb->m_func;
			argv.resize(fb->argv.size());
			for (i = 0; i < fb->argv.size(); i++)
				argv[i] = fb->argv[i];

			v8::Handle<v8::Object> o = fb->wrap();

			v8::TryCatch try_catch;
			func->Call(o, (int) argv.size(), argv.data());
			if (try_catch.HasCaught())
				ReportException(&try_catch, true);

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

result_t startJSFiber(v8::Handle<v8::Function> func, const v8::Arguments& args,
		int nArgStart, obj_ptr<Fiber_base>& retVal)
{
	fiber_data* fb = new fiber_data();
	int i;

	fb->argv.resize(args.Length() - nArgStart);
	for (i = nArgStart; i < args.Length(); i++)
		fb->argv[i - nArgStart] = v8::Persistent<v8::Value>::New(args[i]);
	fb->m_func = v8::Persistent<v8::Function>::New(func);

	g_jobs.put(fb);
	fb->Ref();

	retVal = fb;

	return 0;
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
		AsyncCall ac(args);
		s_acSleep.put(&ac);

		v8::Unlocker unlocker(isolate);
		ac.wait();
	}
	else
	{
		v8::Unlocker unlocker(isolate);
		exlib::Fiber::yield();
	}

	return 0;
}

}
