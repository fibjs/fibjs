/*
 * Fiber.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Fiber.h"
#include "ifs/os.h"

namespace fibjs
{

#define FIBER_PER_CPU	3000
extern v8::Persistent<v8::Context> s_context;

static exlib::Queue<Fiber> g_jobs;
static exlib::IDLE_PROC s_oldIdle;
static int32_t s_cpus;
static int32_t s_fibers;

int g_tlsCurrent;

static class _fiber_init
{
public:
	_fiber_init()
	{
		if (os_base::CPUs(s_cpus) < 0)
			s_cpus = 4;
		s_fibers = 0;

		exlib::Service* pService = exlib::Service::getFiberService();

		g_tlsCurrent = pService->tlsAlloc();
		s_oldIdle = pService->onIdle(onIdle);
	}

	static void onIdle()
	{
		if (!g_jobs.empty() && (s_fibers < s_cpus * FIBER_PER_CPU))
		{
			s_fibers++;
			exlib::Service::CreateFiber(Fiber::fiber_proc)->Unref();
		}

		if (s_oldIdle)
			s_oldIdle();
	}
} s_fiber_init;

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

void* Fiber::fiber_proc(void* p)
{
	exlib::Service* pService = exlib::Service::getFiberService();

	v8::Locker locker(isolate);
	v8::Isolate::Scope isolate_scope(isolate);

	v8::HandleScope handle_scope;
	v8::Context::Scope context_scope(s_context);

	while (1)
	{
		Fiber* fb;

		if ((fb = g_jobs.tryget()) == NULL)
		{
			v8::Unlocker unlocker(isolate);
			fb = g_jobs.get();
		}

		if (fb == NULL)
			break;

		pService->tlsPut(g_tlsCurrent, fb);

		v8::HandleScope handle_scope;

		size_t i;

		std::vector<v8::Handle<v8::Value> > argv;

		argv.resize(fb->argv.size());
		for (i = 0; i < fb->argv.size(); i++)
			argv[i] = fb->argv[i];

		v8::Handle<v8::Object> o = fb->wrap();

		v8::TryCatch try_catch;
		v8::Handle<v8::Value> retVal = fb->m_func->Call(o, (int) argv.size(),
				argv.data());
		if (try_catch.HasCaught())
		{
			v8::Handle<v8::Value> err = try_catch.Exception();
			fb->m_error = true;

			fb->_trigger("error", &err, 1);
			ReportException(&try_catch, true);
			retVal = v8::Null();
		}
		else if (!IsEmpty(retVal))
			fb->m_result = v8::Persistent<v8::Value>::New(retVal);

		fb->_trigger("exit", &retVal, 1);

		fb->m_quit.set();
		fb->dispose();
		fb->Unref();

		s_null.Ref();
		o->SetPointerInInternalField(0, &s_null);
	}

	return NULL;
}

void Fiber::start()
{
	g_jobs.put(this);
	Ref();
}

} /* namespace fibjs */
