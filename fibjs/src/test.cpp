/*
 * test.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: lion
 */

#include "ifs/test.h"
#include "ifs/assert.h"
#include "QuickArray.h"
#include "Expect.h"
#include <log4cpp/Category.hh>
#include "date.h"

#define COLOR_RESET "\x1b[0m"
#define COLOR_BLACK "\x1b[30m" /* Black */
#define COLOR_RED "\x1b[31m" /* Red */
#define COLOR_GREEN "\x1b[32m" /* Green */
#define COLOR_YELLOW "\x1b[33m" /* Yellow */
#define COLOR_BLUE "\x1b[34m" /* Blue */
#define COLOR_MAGENTA "\x1b[35m" /* Magenta */
#define COLOR_CYAN "\x1b[36m" /* Cyan */
#define COLOR_WHITE "\x1b[37m" /* White */
#define COLOR_NORMAL "\x1b[39m" /* Normal */
#define COLOR_GREY "\x1B[90m" /* Grey */
namespace fibjs
{

class _case;

static obj_ptr<_case> s_root;
static obj_ptr<_case> s_now;
static int32_t s_slow = 75;

enum
{
	HOOK_BEFORE = 0, HOOK_AFTER = 1, HOOK_BEFORECASE = 2, HOOK_AFTERCASE = 3,
};

class _case: public obj_base
{
	_case(const char* name = NULL) :
			m_pos(0), m_error(false)
	{
		if (name)
			m_name = name;
	}

	~_case()
	{
		int i, j;

		m_block.Dispose(isolate);
		for (i = 0; i < 4; i++)
			for (j = 0; j < (int) m_hooks[i].size(); j++)
				m_hooks[i][j].Dispose(isolate);
	}

public:
	static void init()
	{
		if (s_root == NULL)
			s_root = s_now = new _case();
	}

	static result_t enter(const char* name)
	{
		_case* now = s_now;
		if (!s_now)
			return CALL_E_INVALID_CALL;

		s_now = new _case(name);
		now->m_subs.append(s_now);
		return 0;
	}

	static result_t it(const char* name, v8::Handle<v8::Function> block)
	{
		_case* now = s_now;
		if (s_now == s_root)
			return CALL_E_INVALID_CALL;

		_case* p = new _case(name);
		p->m_block = v8::Persistent<v8::Function>::New(isolate, block);

		now->m_subs.append(p);
		return 0;
	}

public:
	static result_t set_hook(int type, v8::Handle<v8::Function> func)
	{
		_case* now = s_now;
		if (!s_now)
			return CALL_E_INVALID_CALL;

		now->m_hooks[type].append(
				v8::Persistent<v8::Function>::New(isolate, func));
		return 0;
	}

	static result_t run(int32_t loglevel)
	{
		if (!s_root)
			return 0;

		if (s_now != s_root)
			return CALL_E_INVALID_CALL;

		s_now = NULL;

		QuickArray<obj_ptr<_case> > stack;
		QuickArray<std::string> msgs;
		v8::Handle<v8::Object> o = v8::Object::New();
		int i, j;
		int32_t oldlevel = 0;
		int32_t cnt = 0, errcnt = 0;
		char buf[128];
		date_t da1, da2;

		console_base::get_loglevel(oldlevel);
		console_base::set_loglevel(loglevel);

		stack.append(s_root);

		da1.now();

		while (stack.size())
		{
			_case* p = stack[stack.size() - 1];
			_case* p1, *p2;

			if (p->m_pos == 0)
			{
				for (i = 0; i < (int) p->m_hooks[HOOK_BEFORE].size(); i++)
					if (p->m_hooks[HOOK_BEFORE][i]->Call(o, 0, NULL).IsEmpty())
					{
						console_base::set_loglevel(oldlevel);
						clear();
						return 0;
					}
			}

			if (p->m_pos < (int) p->m_subs.size())
			{
				std::string str(stack.size() * 2, ' ');

				p1 = p->m_subs[p->m_pos++];

				if (p1->m_block.IsEmpty())
				{
					console_base::set_loglevel(oldlevel);
					if (stack.size() == 1)
						asyncLog(log4cpp::Priority::INFO, "");

					str.append(p1->m_name);
					asyncLog(log4cpp::Priority::INFO, str);
					console_base::set_loglevel(loglevel);

					stack.append(p1);
					continue;
				}

				for (j = 0; j < (int) stack.size(); j++)
				{
					p2 = stack[j];
					for (i = 0; i < (int) p2->m_hooks[HOOK_BEFORECASE].size();
							i++)
						if (p2->m_hooks[HOOK_BEFORECASE][i]->Call(o, 0, NULL).IsEmpty())
						{
							console_base::set_loglevel(oldlevel);
							clear();
							return 0;
						}
				}

				cnt++;
				{
					v8::TryCatch try_catch;
					date_t d1, d2;

					d1.now();
					p1->m_block->Call(o, 0, NULL);
					d2.now();

					if (try_catch.HasCaught())
					{
						errcnt++;
						p1->m_error = true;
						if (loglevel > log4cpp::Priority::ERROR)
							ReportException(try_catch, 0);
						else if (loglevel == log4cpp::Priority::ERROR)
							msgs.append(GetException(try_catch, 0));

						sprintf(buf, "%d) ", errcnt);
						str.append(buf);
						str.append(p1->m_name);
					}
					else
					{
						double n = d2.diff(d1);

						str.append(COLOR_GREEN "\xe2\x88\x9a " COLOR_GREY);
						str.append(p1->m_name);
						if (n > s_slow / 2)
						{
							sprintf(buf, " (%dms) ", (int) n);

							if (n > s_slow)
								str.append(COLOR_RED);
							else
								str.append(COLOR_YELLOW);

							str.append(buf);
						}
						str.append(COLOR_NORMAL);
					}
				}

				console_base::set_loglevel(oldlevel);
				asyncLog(
						p1->m_error ?
								log4cpp::Priority::ERROR :
								log4cpp::Priority::INFO, str);
				console_base::set_loglevel(loglevel);

				for (j = (int) stack.size() - 1; j >= 0; j--)
				{
					p2 = stack[j];
					for (i = (int) p2->m_hooks[HOOK_AFTERCASE].size() - 1;
							i >= 0; i--)
						if (p2->m_hooks[HOOK_AFTERCASE][i]->Call(o, 0, NULL).IsEmpty())
						{
							console_base::set_loglevel(oldlevel);
							clear();
							return 0;
						}
				}
			}

			if (p->m_pos == p->m_subs.size())
			{
				for (i = (int) p->m_hooks[HOOK_AFTER].size() - 1; i >= 0; i--)
					if (p->m_hooks[HOOK_AFTER][i]->Call(o, 0, NULL).IsEmpty())
					{
						console_base::set_loglevel(oldlevel);
						clear();
						return 0;
					}
				stack.pop();
			}
		}

		console_base::set_loglevel(oldlevel);
		asyncLog(log4cpp::Priority::INFO, "");

		if (errcnt == 0)
		{
			da2.now();

			sprintf(buf,
					COLOR_GREEN "  \xe2\x88\x9a %d tests completed" COLOR_GREY " (%dms)" COLOR_NORMAL,
					cnt, (int) da2.diff(da1));
			asyncLog(log4cpp::Priority::INFO, buf);
		}
		else
		{
			sprintf(buf, COLOR_RED "  × %d of %d tests failed" COLOR_NORMAL,
					errcnt, cnt);
			asyncLog(log4cpp::Priority::ERROR, buf);
		}

		asyncLog(log4cpp::Priority::INFO, "");

		for (i = 0; i < (int) msgs.size(); i++)
			asyncLog(log4cpp::Priority::ERROR, msgs[i]);

		clear();
		return 0;
	}

	static void clear()
	{
		s_root = NULL;
		s_now = NULL;
	}

private:
	std::string m_name;
	v8::Persistent<v8::Function> m_block;
	QuickArray<obj_ptr<_case> > m_subs;
	QuickArray<v8::Persistent<v8::Function> > m_hooks[4];
	int m_pos;
	bool m_error;
};

result_t test_base::describe(const char* name, v8::Handle<v8::Function> block)
{
	_case::init();

	_case* last = s_now;

	result_t hr = _case::enter(name);
	if (hr < 0)
		return hr;

	block->Call(block->ToObject(), 0, NULL);

	s_now = last;
	return 0;
}

result_t test_base::xdescribe(const char* name, v8::Handle<v8::Function> block)
{
	return 0;
}

result_t test_base::it(const char* name, v8::Handle<v8::Function> block)
{
	return _case::it(name, block);
}

result_t test_base::xit(const char* name, v8::Handle<v8::Function> block)
{
	return 0;
}

result_t test_base::before(v8::Handle<v8::Function> func)
{
	return _case::set_hook(HOOK_BEFORE, func);
}

result_t test_base::after(v8::Handle<v8::Function> func)
{
	return _case::set_hook(HOOK_AFTER, func);
}

result_t test_base::beforeEach(v8::Handle<v8::Function> func)
{
	return _case::set_hook(HOOK_BEFORECASE, func);
}

result_t test_base::afterEach(v8::Handle<v8::Function> func)
{
	return _case::set_hook(HOOK_AFTERCASE, func);
}

result_t test_base::run(int32_t loglevel)
{
	return _case::run(loglevel);
}

result_t test_base::get_assert(obj_ptr<assert_base>& retVal)
{
	static obj_ptr<assert_base> s_assert;

	if (s_assert == NULL)
		s_assert = new assert_base();

	retVal = s_assert;
	return 0;
}

result_t test_base::expect(v8::Handle<v8::Value> actual, const char* msg,
		obj_ptr<Expect_base>& retVal)
{
	retVal = new Expect(actual, msg);
	return 0;
}

result_t test_base::setup(int32_t mode)
{
	v8::Handle < v8::Context > ctx = v8::Context::GetCalling();

	if (!ctx.IsEmpty())
	{
		v8::Context::Scope context_scope(ctx);
		v8::Handle < v8::Object > glob = ctx->Global();
		obj_ptr < assert_base > assert;

		if (mode == _BDD)
		{
			glob->ForceSet(v8::String::New("describe"),
					v8::FunctionTemplate::New(s_describe)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("xdescribe"),
					v8::FunctionTemplate::New(s_xdescribe)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("it"),
					v8::FunctionTemplate::New(s_it)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("xit"),
					v8::FunctionTemplate::New(s_xit)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("before"),
					v8::FunctionTemplate::New(s_before)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("after"),
					v8::FunctionTemplate::New(s_after)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("beforeEach"),
					v8::FunctionTemplate::New(s_describe)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("afterEach"),
					v8::FunctionTemplate::New(s_describe)->GetFunction(),
					v8::ReadOnly);
		}
		else if (mode == _TDD)
		{
			glob->ForceSet(v8::String::New("suite"),
					v8::FunctionTemplate::New(s_describe)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("xsuite"),
					v8::FunctionTemplate::New(s_xdescribe)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("test"),
					v8::FunctionTemplate::New(s_it)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("xtest"),
					v8::FunctionTemplate::New(s_xit)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("suiteSetup"),
					v8::FunctionTemplate::New(s_before)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("suiteTeardown"),
					v8::FunctionTemplate::New(s_after)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("setup"),
					v8::FunctionTemplate::New(s_describe)->GetFunction(),
					v8::ReadOnly);
			glob->ForceSet(v8::String::New("teardown"),
					v8::FunctionTemplate::New(s_describe)->GetFunction(),
					v8::ReadOnly);
		}
		else
			return CALL_E_INVALIDARG;

		glob->ForceSet(v8::String::New("expect"),
				v8::FunctionTemplate::New(s_expect)->GetFunction(),
				v8::ReadOnly);

		get_assert (assert);
		glob->ForceSet(v8::String::New("assert"), assert->wrap(), v8::ReadOnly);
	}

	return 0;
}

result_t test_base::get_slow(int32_t& retVal)
{
	retVal = s_slow;
	return 0;
}

result_t test_base::set_slow(int32_t newVal)
{
	s_slow = newVal;
	return 0;
}

}
