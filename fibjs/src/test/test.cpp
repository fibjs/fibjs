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
#include "date.h"
#include "console.h"

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
    _case(const char *name = NULL) :
        m_pos(0), m_error(false)
    {
        if (name)
            m_name = name;
    }

    ~_case()
    {
        int32_t i, j;

        m_block.Reset();
        for (i = 0; i < 4; i++)
            for (j = 0; j < (int32_t) m_hooks[i].size(); j++)
                m_hooks[i][j].Reset();
    }

public:
    static void init()
    {
        if (s_root == NULL)
            s_root = s_now = new _case();
    }

    static result_t enter(const char *name)
    {
        _case *now = s_now;
        if (!s_now)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        s_now = new _case(name);
        now->m_subs.append(s_now);
        return 0;
    }

    static result_t it(const char *name, v8::Local<v8::Function> block)
    {
        _case *now = s_now;
        if (!now || s_now == s_root)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        _case *p = new _case(name);
        p->m_block.Reset(Isolate::now()->m_isolate, block);

        now->m_subs.append(p);
        return 0;
    }

public:
    static result_t set_hook(int32_t type, v8::Local<v8::Function> func)
    {
        _case *now = s_now;
        if (!s_now)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        QuickArray<v8::Persistent<v8::Function> > &fa = now->m_hooks[type];
        size_t sz = fa.size();

        fa.resize(sz + 1);
        fa[sz].Reset(Isolate::now()->m_isolate, func);

        return 0;
    }

    static result_t run(int32_t loglevel, int32_t& retVal)
    {
        if (!s_root)
            return 0;

        if (s_now != s_root)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        s_now = NULL;

        QuickArray<obj_ptr<_case> > stack;
        QuickArray<std::string> names;
        QuickArray<std::string> msgs;
        int32_t i, j;
        int32_t oldlevel = 0;
        int32_t cnt = 0, errcnt = 0;
        char buf[128];
        date_t da1, da2;

        console_base::get_loglevel(oldlevel);
        console_base::set_loglevel(loglevel);

        stack.append(s_root);

        da1.now();

        Isolate* isolate = Isolate::now();

        while (stack.size())
        {
            _case *p = stack[stack.size() - 1];
            _case *p1, *p2;

            if (p->m_pos == 0)
            {
                for (i = 0; i < (int32_t) p->m_hooks[HOOK_BEFORE].size(); i++)
                    if (v8::Local<v8::Function>::New(isolate->m_isolate,
                                                     p->m_hooks[HOOK_BEFORE][i])->Call(v8::Undefined(isolate->m_isolate),
                                                             0, NULL).IsEmpty())
                    {
                        console_base::set_loglevel(oldlevel);
                        clear();
                        return 0;
                    }
            }

            if (p->m_pos < (int32_t) p->m_subs.size())
            {
                std::string str(stack.size() * 2, ' ');

                p1 = p->m_subs[p->m_pos++];

                if (p1->m_block.IsEmpty())
                {
                    console_base::set_loglevel(oldlevel);
                    if (stack.size() == 1)
                        asyncLog(console_base::_INFO, "");

                    str.append(logger::highLight());
                    str.append(p1->m_name);
                    str.append(COLOR_RESET);

                    asyncLog(console_base::_INFO, str);
                    console_base::set_loglevel(loglevel);

                    stack.append(p1);
                    continue;
                }

                for (j = 0; j < (int32_t) stack.size(); j++)
                {
                    p2 = stack[j];
                    for (i = 0; i < (int32_t) p2->m_hooks[HOOK_BEFORECASE].size();
                            i++)
                        if (v8::Local<v8::Function>::New(isolate->m_isolate,
                                                         p2->m_hooks[HOOK_BEFORECASE][i])->Call(v8::Undefined(isolate->m_isolate),
                                                                 0, NULL).IsEmpty())
                        {
                            console_base::set_loglevel(oldlevel);
                            clear();
                            return 0;
                        }
                }

                cnt++;
                {
                    TryCatch try_catch;
                    date_t d1, d2;

                    d1.now();
                    v8::Local<v8::Function>::New(isolate->m_isolate, p1->m_block)->Call(v8::Undefined(isolate->m_isolate),
                            0, NULL);
                    d2.now();

                    if (try_catch.HasCaught())
                    {
                        sprintf(buf, "%d) ", ++errcnt);

                        p1->m_error = true;
                        if (loglevel > console_base::_ERROR)
                            ReportException(try_catch, 0);
                        else if (loglevel == console_base::_ERROR)
                        {
                            std::string str1(buf);

                            for (i = 1; i < (int32_t)stack.size(); i ++)
                            {
                                str1.append(stack[i]->m_name);
                                str1.append(" ", 1);
                            }
                            str1.append(p1->m_name);
                            names.append(logger::highLight() + str1 + COLOR_RESET);

                            msgs.append(GetException(try_catch, 0));
                        }

                        str.append(buf);
                        str.append(p1->m_name);
                    }
                    else
                    {
                        double n = d2.diff(d1);

                        str.append(logger::notice() + "\xe2\x88\x9a " COLOR_RESET);
                        str.append(p1->m_name);
                        if (n > s_slow / 2)
                        {
                            sprintf(buf, " (%dms) ", (int32_t) n);

                            if (n > s_slow)
                                str.append(logger::error());
                            else
                                str.append(logger::warn());

                            str.append(buf);
                            str.append(COLOR_RESET);
                        }
                    }
                }

                console_base::set_loglevel(oldlevel);
                asyncLog(
                    p1->m_error ?
                    console_base::_ERROR :
                    console_base::_INFO, str);
                console_base::set_loglevel(loglevel);

                for (j = (int32_t) stack.size() - 1; j >= 0; j--)
                {
                    p2 = stack[j];
                    for (i = (int32_t) p2->m_hooks[HOOK_AFTERCASE].size() - 1;
                            i >= 0; i--)
                        if (v8::Local<v8::Function>::New(isolate->m_isolate,
                                                         p2->m_hooks[HOOK_AFTERCASE][i])->Call(v8::Undefined(isolate->m_isolate),
                                                                 0, NULL).IsEmpty())
                        {
                            console_base::set_loglevel(oldlevel);
                            clear();
                            return 0;
                        }
                }
            }

            if (p->m_pos == (int32_t)p->m_subs.size())
            {
                for (i = (int32_t) p->m_hooks[HOOK_AFTER].size() - 1; i >= 0; i--)
                    if (v8::Local<v8::Function>::New(isolate->m_isolate,
                                                     p->m_hooks[HOOK_AFTER][i])->Call(v8::Undefined(isolate->m_isolate),
                                                             0, NULL).IsEmpty())
                    {
                        console_base::set_loglevel(oldlevel);
                        clear();
                        return 0;
                    }
                stack.pop();
            }
        }

        console_base::set_loglevel(oldlevel);
        asyncLog(console_base::_INFO, "");

        if (errcnt == 0)
        {
            da2.now();

            sprintf(buf,
                    (logger::notice() + "  \xe2\x88\x9a %d tests completed" COLOR_RESET " (%dms)").c_str(),
                    cnt, (int32_t) da2.diff(da1));
            asyncLog(console_base::_INFO, buf);
        }
        else
        {
            sprintf(buf, (logger::error() + "  × %d of %d tests failed" COLOR_RESET).c_str(),
                    errcnt, cnt);
            asyncLog(console_base::_ERROR, buf);
        }

        asyncLog(console_base::_INFO, "");

        for (i = 0; i < (int32_t) msgs.size(); i++)
        {
            asyncLog(console_base::_INFO, names[i]);
            asyncLog(console_base::_ERROR, msgs[i]);
        }

        clear();

        retVal = errcnt;

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
    int32_t m_pos;
    bool m_error;
};

result_t test_base::describe(const char *name, v8::Local<v8::Function> block)
{
    _case::init();

    _case *last = s_now;

    result_t hr = _case::enter(name);
    if (hr < 0)
        return hr;

    block->Call(v8::Undefined(Isolate::now()->m_isolate), 0, NULL);

    s_now = last;
    return 0;
}

result_t test_base::xdescribe(const char *name, v8::Local<v8::Function> block)
{
    return 0;
}

result_t test_base::it(const char *name, v8::Local<v8::Function> block)
{
    return _case::it(name, block);
}

result_t test_base::xit(const char *name, v8::Local<v8::Function> block)
{
    return 0;
}

result_t test_base::before(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_BEFORE, func);
}

result_t test_base::after(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_AFTER, func);
}

result_t test_base::beforeEach(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_BEFORECASE, func);
}

result_t test_base::afterEach(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_AFTERCASE, func);
}

result_t test_base::run(int32_t loglevel, int32_t& retVal)
{
    return _case::run(loglevel, retVal);
}

result_t test_base::expect(v8::Local<v8::Value> actual, const char *msg,
                           obj_ptr<Expect_base> &retVal)
{
    retVal = new Expect(actual, msg);
    return 0;
}

result_t test_base::setup(int32_t mode)
{
    Isolate* isolate = Isolate::now();
    if (isolate->m_test_setup_bbd && isolate->m_test_setup_tdd)
        return 0;

    v8::Local<v8::Object> glob = v8::Local<v8::Object>::New(isolate->m_isolate, isolate->m_global);
    v8::Local<v8::Context> _context = v8::Local<v8::Context>::New(isolate->m_isolate, isolate->m_context);

    if (!isolate->m_test_setup_bbd && !isolate->m_test_setup_tdd)
    {
        glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "expect"),
                                v8::Function::New(isolate->m_isolate, s_expect),
                                (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));

        glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "assert"),
                                assert_base::class_info().getFunction(),
                                (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
    }

    if (mode == _BDD)
    {
        if (!isolate->m_test_setup_bbd)
        {
            isolate->m_test_setup_bbd = true;

            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "describe"),
                                    v8::Function::New(isolate->m_isolate, s_describe),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "xdescribe"),
                                    v8::Function::New(isolate->m_isolate, s_xdescribe),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "it"),
                                    v8::Function::New(isolate->m_isolate, s_it),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "xit"),
                                    v8::Function::New(isolate->m_isolate, s_xit),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "before"),
                                    v8::Function::New(isolate->m_isolate, s_before),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "after"),
                                    v8::Function::New(isolate->m_isolate, s_after),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "beforeEach"),
                                    v8::Function::New(isolate->m_isolate, s_beforeEach),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "afterEach"),
                                    v8::Function::New(isolate->m_isolate, s_afterEach),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
        }
    }
    else if (mode == _TDD)
    {
        if (!isolate->m_test_setup_tdd)
        {
            isolate->m_test_setup_tdd = true;

            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "suite"),
                                    v8::Function::New(isolate->m_isolate, s_describe),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "xsuite"),
                                    v8::Function::New(isolate->m_isolate, s_xdescribe),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "test"),
                                    v8::Function::New(isolate->m_isolate, s_it),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "xtest"),
                                    v8::Function::New(isolate->m_isolate, s_xit),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "suiteSetup"),
                                    v8::Function::New(isolate->m_isolate, s_before),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "suiteTeardown"),
                                    v8::Function::New(isolate->m_isolate, s_after),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "setup"),
                                    v8::Function::New(isolate->m_isolate, s_beforeEach),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            glob->DefineOwnProperty(_context, v8::String::NewFromUtf8(isolate->m_isolate, "teardown"),
                                    v8::Function::New(isolate->m_isolate, s_afterEach),
                                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
        }
    }
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

result_t test_base::get_slow(int32_t &retVal)
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
