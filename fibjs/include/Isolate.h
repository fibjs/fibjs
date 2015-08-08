/*
 * Isolate.h
 *
 *  Created on: May 2, 2015
 *      Author: lion
 */

#ifndef ISOLATE_H_
#define ISOLATE_H_

#include <exlib/include/list.h>
#include <exlib/include/service.h>

namespace fibjs
{

class SandBox;
class JSFiber;
class Isolate : public exlib::linkitem
{
public:
	class rt
	{
	public:
		rt();
		~rt();

	public:
		static bool g_trace;

	private:
		JSFiber *m_fiber;
		v8::Unlocker unlocker;
	};

public:
	static Isolate* now();
	static void reg(void *rt);

public:
	exlib::Service *m_service;
	v8::Isolate *m_isolate;
	v8::Persistent<v8::Context> m_context;
	v8::Persistent<v8::Object> m_global;
	obj_ptr<SandBox> m_topSandbox;
	exlib::List<exlib::linkitem> m_fibers;
};

} /* namespace fibjs */
#endif /* ISOLATE_H_ */
