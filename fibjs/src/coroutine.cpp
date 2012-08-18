/*
 * coroutine.cpp
 *
 *  Created on: Apr 9, 2012
 *      Author: lion
 */

#include "ifs/coroutine.h"
#include "ifs/os.h"
#include "Fiber.h"
#include <vector>

namespace fibjs
{

result_t coroutine_base::start(v8::Handle<v8::Function> func,
		const v8::Arguments& args, obj_ptr<Fiber_base>& retVal)
{
	return Fiber::startJSFiber(func, args, 1, retVal);
}

inline result_t _parallel(std::vector<v8::Handle<v8::Function> >& funs,
		v8::Handle<v8::Array>& retVal)
{
	std::vector<obj_ptr<Fiber_base> > fibers;
	int i;

	fibers.resize(funs.size());
	for (i = 1; i < (int) funs.size(); i++)
		Fiber::startJSFiber(funs[i], NULL, 0, fibers[i]);

	coroutine_base::current(fibers[0]);
	v8::Handle<v8::Object> o = fibers[0]->wrap();

	bool bError = false;
	v8::TryCatch try_catch;
	v8::Handle<v8::Value> r = funs[0]->Call(o, 0, NULL);
	if (try_catch.HasCaught())
	{
		bError = true;
		ReportException(&try_catch, true);
		r = v8::Undefined();
	}

	retVal = v8::Array::New((int) funs.size());
	retVal->Set(0, r);

	for (i = 1; i < (int) funs.size(); i++)
	{
		Fiber* fb = (Fiber*) (Fiber_base*) fibers[i];
		fb->join();

		if (fb->isError())
			bError = true;
		else if (!bError && fb->get_result(r) != CALL_RETURN_NULL)
			retVal->Set(i, r);
	}

	if (bError)
		return CALL_E_INTERNAL;

	return 0;
}

result_t coroutine_base::parallel(v8::Handle<v8::Array> func,
		v8::Handle<v8::Array>& retVal)
{
	int l = func->Length();
	int i;

	if (l == 0)
		return CALL_RETURN_NULL;

	std::vector<v8::Handle<v8::Function> > funs;
	funs.resize(l);

	for (i = 0; i < l; i++)
	{
		v8::Handle<v8::Value> v = func->Get(i);

		if (!v.IsEmpty() && v->IsFunction())
			funs[i] = v8::Handle<v8::Function>::Cast(v);
		else
			return CALL_E_INVALIDARG;
	}

	return _parallel(funs, retVal);
}

result_t coroutine_base::parallel(const v8::Arguments& args,
		v8::Handle<v8::Array>& retVal)
{
	int l = args.Length();
	int i;

	if (l == 0)
		return CALL_RETURN_NULL;

	std::vector<v8::Handle<v8::Function> > funs;
	funs.resize(l);

	for (i = 0; i < l; i++)
	{
		v8::Handle<v8::Value> v = args[i];

		if (!v.IsEmpty() && v->IsFunction())
			funs[i] = v8::Handle<v8::Function>::Cast(v);
		else
			return CALL_E_INVALIDARG;
	}

	return _parallel(funs, retVal);
}

result_t coroutine_base::current(obj_ptr<Fiber_base>& retVal)
{
	exlib::Service* pService = exlib::Service::getFiberService();

	if (pService)
	{
		Fiber_base* fb = (Fiber_base*) pService->tlsGet(g_tlsCurrent);

		if (!fb)
		{
			fb = new Fiber();
			pService->tlsPut(g_tlsCurrent, fb);
			fb->Ref();
		}

		retVal = fb;
	}

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
