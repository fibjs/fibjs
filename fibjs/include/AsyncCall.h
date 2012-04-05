#ifndef _fj_ASYNCCALL_H
#define _fj_ASYNCCALL_H

#include <exlib/fiber.h>
#include <exlib/lockfree.h>
#include "utils.h"

namespace fibjs
{

class _AsyncCall: public exlib::AsyncEvent
{
public:
	_AsyncCall(void ** a, void(*f)(_AsyncCall*) = NULL) :
		func(f), args(a), hr(0)
	{
	}

public:
	void (*func)(_AsyncCall*);
	void ** args;
	result_t hr;
};

typedef exlib::lockfree<_AsyncCall> AsyncQueue;

template<typename T1, typename T2>
result_t AryncCallProc(AsyncQueue& q, result_t(*func)(T1 v1, T2&v2), T1& v1,
		T2& v2)
{
	class _t
	{
	public:
		static void _stub(_AsyncCall* ac)
		{
			ac->hr = ((result_t(*)(T1 v1, T2&v2)) ac->args[0])(
					*(const char**) ac->args[1], *(std::string*) ac->args[2]);
			ac->post();
		}
	};

	void* args[] =
	{ (void*) func, &v1, &v2 };
	_AsyncCall ac(args, _t::_stub);

	q.put(&ac);
	ac.weak.wait();

	return 0;
}

}

#endif
