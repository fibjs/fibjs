/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "Fiber.h"
#include "SandBox.h"
#include "console.h"
#include "ifs/global.h"
#include "ifs/process.h"

namespace fibjs
{

static int32_t s_tls_rt;

void init_rt()
{
	s_tls_rt = exlib::Fiber::tlsAlloc();
}

void Runtime::reg()
{
	exlib::Fiber::tlsPut(s_tls_rt, this);
}

Runtime &Runtime::current()
{
	return *(Runtime *)exlib::Fiber::tlsGet(s_tls_rt);
}

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
	virtual void* Allocate(size_t length)
	{
		void* data = AllocateUninitialized(length);
		return data == NULL ? data : memset(data, 0, length);
	}

	virtual void* AllocateUninitialized(size_t length)
	{
		return malloc(length);
	}

	virtual void Free(void* data, size_t)
	{
		free(data);
	}
};

exlib::LockedList<Isolate> s_isolates;
exlib::atomic s_iso_id;
extern int32_t stack_size;

bool Isolate::rt::g_trace = false;

inline JSFiber* saveTrace()
{
	JSFiber* fiber = JSFiber::current();
	assert(fiber != 0);
	fiber->m_traceInfo = traceInfo(300);
	return fiber;
}

Isolate::rt::rt() :
	m_fiber(g_trace ? saveTrace() : NULL),
	unlocker(m_isolate->m_isolate)
{
}

Isolate::rt::~rt()
{
	if (m_fiber)
		m_fiber->m_traceInfo.resize(0);
}

static void delay_gc(Isolate *isolate)
{
	exlib::linkitem* p;

	while ((p = isolate->m_free.getHead()) != 0)
		object_base::gc_delete(p);
}

static void fb_GCCallback(v8::Isolate* js_isolate, v8::GCType type, v8::GCCallbackFlags flags)
{
	Isolate *isolate = Isolate::current();
	exlib::linkitem* p;

	isolate->m_weakLock.lock();
	while ((p = isolate->m_weak.getHead()) != 0)
		if (!object_base::gc_weak(p))
			isolate->m_free.putTail(p);
	isolate->m_weakLock.unlock();

	syncCall(isolate, delay_gc, isolate);
}

void *init_proc(void *p)
{
	Isolate* isolate = (Isolate*)p;
	Runtime rt(isolate);

	isolate->init();
	return FiberBase::fiber_proc(p);
}

Isolate::Isolate(const char *fname) :
	m_id((int32_t)s_iso_id.inc()),
	m_test_setup_bbd(false), m_test_setup_tdd(false), m_test(NULL),
	m_currentFibers(0), m_idleFibers(0),
	m_loglevel(console_base::_NOTSET), m_interrupt(false)
{
	if (fname)
		m_fname = fname;

	static v8::Isolate::CreateParams create_params;
	static ShellArrayBufferAllocator array_buffer_allocator;
	create_params.array_buffer_allocator = &array_buffer_allocator;

	m_isolate = v8::Isolate::New(create_params);
	m_isolate->AddGCPrologueCallback(fb_GCCallback, v8::kGCTypeMarkSweepCompact);

	m_currentFibers++;
	m_idleFibers ++;

	exlib::Service::Create(init_proc, this, stack_size * 1024, "JSFiber");
}

Isolate* Isolate::current()
{
	return Runtime::current().isolate();
}

void Isolate::init()
{
	s_isolates.putTail(this);

	v8::Locker locker(m_isolate);
	v8::Isolate::Scope isolate_scope(m_isolate);
	v8::HandleScope handle_scope(m_isolate);

	v8::Local<v8::Context> _context = v8::Context::New(m_isolate);
	m_context.Reset(m_isolate, _context);

	v8::Local<v8::Object> glob = _context->Global();
	m_global.Reset(m_isolate, glob);

	v8::Context::Scope context_scope(_context);

	m_topSandbox = new SandBox();
	m_topSandbox->initRoot();

	static const char* skips[] = {"repl", "argv", NULL};
	global_base::class_info().Attach(this, glob, skips);
}

} /* namespace fibjs */

