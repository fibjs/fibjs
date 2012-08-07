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
