/*
 * AsyncCall.cpp
 *
 *  Created on: May 9, 2012
 *      Author: lion
 */

#include "object.h"

namespace fibjs
{

extern v8::Persistent<v8::Context> s_context;
static exlib::Queue<exlib::AsyncEvent> g_cbs;

static class _callback_init
{
public:
	_callback_init()
	{
		exlib::Service::CreateFiber(_callback)->Unref();
	}

	static void* _callback(void* p)
	{
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolate_scope(isolate);

		v8::HandleScope handle_scope;
		v8::Context::Scope context_scope(s_context);

		while (1)
		{
			v8::HandleScope handle_scope;
			asyncEvent* ac;

			if ((ac = (asyncEvent*) g_cbs.tryget()) == NULL)
			{
				v8::Unlocker unlocker(isolate);
				ac = (asyncEvent*) g_cbs.get();
			}

			if (ac == NULL)
				break;

			ac->callback();
		}

		return NULL;
	}
} s_callback_init;

void AsyncCallBack::invoke()
{
	g_cbs.put(this);
}

void asyncCallBack::invoke()
{
	g_cbs.put(this);
}

}
