/*
 *  mem_trace.cpp
 *  Created on: Aug 4, 2015
 *
 *  Copyright (c) 2015 by Leo Hoo
 *  lion@9465.net
 */

#include <stdlib.h>
#include <stdint.h>

#if !defined(_WIN32) && defined(DEBUG)

#include <cxxabi.h>
#include <dlfcn.h>
#include <unistd.h>
#include <vector>
#include <qstring.h>

#include <exlib/include/list.h>
#include <exlib/include/fiber.h>
#include <exlib/include/service.h>

#if !defined(__clang__)

#define wrap(str) __wrap_##str

extern "C" void* __real_malloc(size_t);
extern "C" void __real_free(void*);
extern "C" void* __real_realloc(void* p, size_t sz);

inline void init_lib()
{}

#else

#define wrap(str) str

static void *(*__real_malloc)(size_t);
static void (*__real_free)(void*);
static void* (*__real_realloc)(void* p, size_t sz);

inline void init_lib()
{
	if (__real_malloc == 0)
		__real_malloc = (void *(*)(size_t))dlsym(RTLD_NEXT, "malloc");
	if (__real_free == 0)
		__real_free = (void (*)(void*))dlsym(RTLD_NEXT, "free");
	if (__real_realloc == 0)
		__real_realloc = (void* (*)(void* p, size_t sz))dlsym(RTLD_NEXT, "realloc");
}
#endif

#ifdef i386
#define get_bp(bp) asm("movl %%ebp, %0" : "=r" (bp) :)
#endif

#ifdef amd64
#define get_bp(bp) asm("movq %%rbp, %0" : "=r" (bp) :)
#endif

namespace fibjs
{

inline void out_proc(FILE* fp, void * proc)
{
	Dl_info info;
	char* demangled = NULL;

	if (proc == 0)
		fprintf(fp, "null\n");
	else if (!dladdr(proc, &info) || !info.dli_sname)
		fprintf(fp, "%p\n", proc);
	else if ((demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, 0))) {
		fprintf(fp, "%s + %ld\n", demangled, (intptr_t)proc - (intptr_t)info.dli_saddr);
		__real_free(demangled);
	} else
		fprintf(fp, "%s + %ld\n", info.dli_sname, (intptr_t)proc - (intptr_t)info.dli_saddr);
}

inline void out_size(FILE* fp, size_t sz)
{
	double num = (double)sz;

	if (num < 1024)
		fprintf(fp, "%lu bytes\n", sz);
	else if (num < 1024 * 1024)
		fprintf(fp, "%.1f KB\n", num / 1024);
	else if (num < 1024 * 1024 * 1024)
		fprintf(fp, "%.1f MB\n", num / (1024 * 1024));
	else
		fprintf(fp, "%.1f GB\n", num / (1024 * 1024 * 1024));
}

class StackFrame : public exlib::linkitem
{
private:
	struct stack_frame {
		struct stack_frame* next;
		void* ret;
	};

	const int32_t stack_align_mask = sizeof(intptr_t) - 1;

public:
	void save(size_t sz, void* bp)
	{
		m_sz = sz;

		void* st = 0;

		exlib::Thread_base* fb = exlib::Thread_base::current();
		if (fb)
			st = (void*)fb->stackguard();

		stack_frame* frame = (stack_frame*)bp;
		m_frame_count = 0;

		if ((((intptr_t)frame & stack_align_mask) != 0) ||
		        (st && frame >= st) ||
		        frame->ret == 0)
			return;

		while (frame && m_frame_count < (int32_t)ARRAYSIZE(m_frames)) {
			m_frames[m_frame_count++] = frame->ret;

			stack_frame* frame1 = frame->next;
			if (frame1 < frame ||
			        (((intptr_t)frame1 & stack_align_mask) != 0) ||
			        (st && frame1 >= st) ||
			        frame1->ret == 0)
				break;
			frame = frame1;
		}
	}

	void save(size_t sz)
	{
		void* bp;
		get_bp(bp);
		save(sz, bp);
	}

public:
	size_t m_sz;
	void* m_frames[100];
	int32_t m_frame_count;
};

class MemPool
{
public:
	class caller
	{
	public:
		caller(void *p): proc(p), m_times(0), m_sz(0)
		{
		}

		~caller()
		{
			int32_t i;
			for (i = 0; i < (int32_t)subs.size(); i ++)
				delete subs[i];
		}

	public:
		void put(size_t sz, void** frames, int32_t level)
		{
			m_times ++;
			m_sz += sz;

			if (level > 0) {
				int32_t i;
				void* p = frames[0];
				caller* sub;

				for (i = 0; i < (int32_t)subs.size(); i ++)
				{
					sub = subs[i];
					if (sub->proc == p)
						break;
				}

				if (i == (int32_t)subs.size()) {
					sub = new caller(p);
					subs.push_back(sub);
				}
				sub->put(sz, frames + 1, level - 1);
			}
		}

		static int32_t compare (const void * a, const void * b)
		{
			return (*(caller**)b)->m_sz - (*(caller**)a)->m_sz;
		}

		void dumpSubs(FILE* fp, int32_t level = 0)
		{
			int32_t i;

			qsort(subs.data(), subs.size(), sizeof(caller*), compare);
			for (i = 0; i < (int32_t)subs.size(); i ++)
				subs[i]->dump(fp, level);
		}

		void dump(FILE* fp, int32_t level)
		{
			qstring str;

			if (level == 0)
				fprintf(fp, "\n");

			str.append(level * 4, ' ');

			fprintf(fp, "%s%d times, total ", str.c_str(), m_times);
			out_size(fp, m_sz);

			str.append(4, ' ');
			fprintf(fp, "%s", str.c_str());

			out_proc(fp, proc);

			caller* p = this;
			while (p->subs.size() == 1)
			{
				p = p->subs[0];
				fprintf(fp, "%s", str.c_str());
				out_proc(fp, p->proc);
			}

			if (p && p->subs.size() > 0)
				p->dumpSubs(fp, level + 1);
		}

	public:
		void * proc;
		int32_t m_times;
		size_t m_sz;
		std::vector<caller*> subs;
	};

public:
	static MemPool& global()
	{
		static MemPool s_global;
		return s_global;
	}

	void add(void* i, size_t sz)
	{
		((StackFrame*)i)->save(sz);
		add(i);
	}

	void add(void* i)
	{
		m_lock.lock();
		m_list.putTail((StackFrame*)i);
		m_lock.unlock();
	}

	void remove(void* i)
	{
		m_lock.lock();
		m_list.remove((StackFrame*)i);
		m_lock.unlock();
	}

	void dump()
	{
		StackFrame* items;
		StackFrame* p;
		int32_t n = 0;
		int32_t i;
		char fname[32];

		init_lib();

		m_lock.lock();
		items = (StackFrame*)__real_malloc(sizeof(StackFrame) * m_list.count());

		p = m_list.head();
		while (p)
		{
			memcpy(items + n ++, p, sizeof(StackFrame));
			p = m_list.next(p);
		}
		m_lock.unlock();

		caller root(0);
		for (i = 0; i < n; i ++)
			root.put(items[i].m_sz, items[i].m_frames, items[i].m_frame_count);

		sprintf(fname, "fibjs.%d.heap", getpid());
		FILE* fp = fopen(fname, "w");
		if (fp)
		{
			fprintf(fp, "\nfound %d times, total ", root.m_times);
			out_size(fp, root.m_sz);
			root.dumpSubs(fp);
			fclose(fp);
		}

		__real_free(items);
	}

public:
	exlib::List<StackFrame> m_list;
	exlib::spinlock m_lock;
};

void dump_memory()
{
	MemPool::global().dump();
}

static FILE* fp;
static int32_t fiber_num = 0;

void dump_one(exlib::Fiber* fb)
{
	StackFrame sf;
	int32_t i;

	fprintf(fp, "\nFiber %d %s\n", fiber_num ++, fb->name());
	sf.save(0, (void*)fb->m_cntxt.fp);
	for (i = 0; i < sf.m_frame_count; i ++)
	{
		fprintf(fp, "    ");
		out_proc(fp, sf.m_frames[i]);
	}
}

void dump_stack()
{
	char fname[32];

	sprintf(fname, "fibjs.%d.stack", getpid());
	fp = fopen(fname, "w");
	if (fp)
	{
		exlib::Service::forEach(dump_one);
		fclose(fp);
	}
}

}

#define STUB_SIZE	((sizeof(fibjs::StackFrame) + 0x1f) & 0xfffffe0)
#define FULL_SIZE(sz)	((sz) + STUB_SIZE)
#define MEM_PTR(p)	((p) ? (void*)((intptr_t)(p) + STUB_SIZE) : 0)
#define STUB_PTR(p)	((p) ? (void*)((intptr_t)(p) - STUB_SIZE) : 0)

extern "C" void* wrap(malloc)(size_t sz)
{
	init_lib();

	void* p1 = __real_malloc(FULL_SIZE(sz));

	if (p1) {
		memset(p1, 0, STUB_SIZE);
		fibjs::MemPool::global().add(p1, sz);
	}

	return MEM_PTR(p1);
}

extern "C" void wrap(free)(void *p)
{
	init_lib();

	void* p1 = STUB_PTR(p);

	if (p1)
		fibjs::MemPool::global().remove(p1);

	__real_free(p1);
}

extern "C" void* wrap(realloc)(void* p, size_t sz)
{
	init_lib();

	fibjs::MemPool& mp = fibjs::MemPool::global();

	if (p == 0)
	{
		void* p1 = __real_malloc(FULL_SIZE(sz));

		if (p1) {
			memset(p1, 0, STUB_SIZE);
			mp.add(p1, sz);
		}

		return MEM_PTR(p1);
	}

	if (sz == 0)
	{
		void* p1 = STUB_PTR(p);

		if (p1)
			mp.remove(p1);

		__real_free(p1);
		return 0;
	}

	void* p1 = STUB_PTR(p);

	mp.remove(p1);
	void* p2 = __real_realloc(p1, FULL_SIZE(sz));
	if (p2) {
		memset(p2, 0, STUB_SIZE);
		mp.add(p2, sz);
	}
	else
		mp.add(p1);

	return MEM_PTR(p2);
}

extern "C" void* wrap(calloc)(size_t num, size_t sz)
{
	init_lib();

	void* p = wrap(malloc)(num * sz);
	if (p)
		memset(p, 0, num * sz);

	return p;
}

void* operator new (size_t sz)
{
	init_lib();

	void* p1 = __real_malloc(FULL_SIZE(sz));

	if (p1) {
		memset(p1, 0, STUB_SIZE);
		fibjs::MemPool::global().add(p1, sz);
	}

	return MEM_PTR(p1);
}

void* operator new[] (size_t sz)
{
	init_lib();

	void* p1 = __real_malloc(FULL_SIZE(sz));

	if (p1) {
		memset(p1, 0, STUB_SIZE);
		fibjs::MemPool::global().add(p1, sz);
	}

	return MEM_PTR(p1);
}

void operator delete (void* p) throw()
{
	init_lib();

	wrap(free)(p);
}

void operator delete[] (void* p) throw()
{
	init_lib();

	wrap(free)(p);
}

#else

namespace fibjs
{

void dump_memory()
{
}

void dump_stack()
{
}

}

#endif
