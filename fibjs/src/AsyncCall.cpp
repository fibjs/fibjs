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
static exlib::Queue<exlib::AsyncEvent> g_oar;

static class _callback_init
{
public:
	_callback_init()
	{
		exlib::Service::CreateFiber(_release)->Unref();
		exlib::Service::CreateFiber(_trigger)->Unref();
	}

	static void* _release(void* p)
	{
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolate_scope(isolate);

		v8::HandleScope handle_scope;
		v8::Context::Scope context_scope(s_context);

		while (1)
		{
			v8::HandleScope handle_scope;
			object_base::asyncRelease* oar;

			if ((oar = (object_base::asyncRelease*) g_oar.tryget()) == NULL)
			{
				v8::Unlocker unlocker(isolate);
				oar = (object_base::asyncRelease*) g_oar.get();
			}

			if (oar == NULL)
				break;

			oar->release();
		}

		return NULL;
	}

	static void* _trigger(void* p)
	{
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolate_scope(isolate);

		v8::HandleScope handle_scope;
		v8::Context::Scope context_scope(s_context);

		while (1)
		{
			v8::HandleScope handle_scope;
			AsyncCallBack* ac;

			if ((ac = (AsyncCallBack*) g_cbs.tryget()) == NULL)
			{
				v8::Unlocker unlocker(isolate);
				ac = (AsyncCallBack*) g_cbs.get();
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

void object_base::asyncRelease::invoke()
{
	g_oar.put(this);
}

}
