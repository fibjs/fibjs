/*
 * test.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: lion
 */

#include "object.h"
#include "ifs/test.h"
#include "ifs/assert.h"
#include "ifs/util.h"
#include "QuickArray.h"
#include "date.h"
#include "console.h"
#include "parse.h"

namespace fibjs {

DECLARE_MODULE(test);

class _case;

static int32_t s_slow = 75;

void asyncLog(int32_t priority, exlib::string msg);

class TestData {
public:
    obj_ptr<_case> m_root;
    obj_ptr<_case> m_now;

    static TestData* current()
    {
        Isolate* isolate = Isolate::current();
        TestData* td = (TestData*)isolate->m_test;
        if (!td)
            isolate->m_test = td = new TestData();

        return td;
    }
};

enum {
    HOOK_BEFORE = 0,
    HOOK_AFTER = 1,
    HOOK_BEFORECASE = 2,
    HOOK_AFTERCASE = 3,
};

class _case : public obj_base {
    _case(exlib::string name = "", int32_t level = TEST_NORMAL)
        : m_level(level)
        , m_run_level(TEST_NORMAL)
        , m_pos(0)
        , m_error(false)
    {
        m_name = name;
    }

    ~_case()
    {
        int32_t i, j;

        m_block.Reset();
        for (i = 0; i < 4; i++)
            for (j = 0; j < (int32_t)m_hooks[i].size(); j++)
                m_hooks[i][j].Reset();
    }

public:
    enum {
        TEST_SKIP = 0,
        TEST_NORMAL = 1,
        TEST_ONLY = 2
    };

public:
    void append(_case* c)
    {
        m_subs.append(c);
        if (c->m_level > m_run_level)
            m_run_level = c->m_level;
    }

public:
    static void init()
    {
        TestData* td = TestData::current();

        if (td->m_root == NULL)
            td->m_root = td->m_now = new _case();
    }

    static result_t describe(exlib::string name, v8::Local<v8::Function> block, int32_t level)
    {
        TestData* td = TestData::current();

        _case::init();

        _case* now = td->m_now;
        if (!now)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        _case* p = new _case(name, level);
        now->append(p);

        td->m_now = p;
        block->Call(v8::Undefined(Isolate::current()->m_isolate), 0, NULL);
        td->m_now = now;

        return 0;
    }

    static result_t it(exlib::string name, v8::Local<v8::Function> block, int32_t level)
    {
        TestData* td = TestData::current();

        _case* now = td->m_now;
        if (!now || td->m_now == td->m_root)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        _case* p = new _case(name, level);
        p->m_block.Reset(Isolate::current()->m_isolate, block);

        now->append(p);
        return 0;
    }

public:
    static result_t set_hook(int32_t type, v8::Local<v8::Function> func)
    {
        TestData* td = TestData::current();

        _case* now = td->m_now;
        if (!td->m_now)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        QuickArray<v8::Global<v8::Function>>& fa = now->m_hooks[type];
        size_t sz = fa.size();

        fa.resize(sz + 1);
        fa[sz].Reset(Isolate::current()->m_isolate, func);

        return 0;
    }

    static result_t run(int32_t loglevel, int32_t& retVal)
    {
        TestData* td = TestData::current();

        if (!td->m_root)
            return 0;

        if (td->m_now != td->m_root)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        td->m_now = NULL;

        QuickArray<obj_ptr<_case>> stack;
        QuickArray<exlib::string> names;
        QuickArray<exlib::string> msgs;
        int32_t i, j;
        int32_t oldlevel = 0;
        int32_t cnt = 0, errcnt = 0;
        char buf[128];
        date_t da1, da2;

        coroutine_base::get_loglevel(oldlevel);
        coroutine_base::set_loglevel(loglevel);

        stack.append(td->m_root);

        da1.now();

        Isolate* isolate = Isolate::current();

        while (stack.size()) {
            _case* p = stack[stack.size() - 1];
            _case *p1, *p2;

            if (p->m_pos == 0) {
                for (i = 0; i < (int32_t)p->m_hooks[HOOK_BEFORE].size(); i++)
                    if (v8::Local<v8::Function>::New(isolate->m_isolate,
                            p->m_hooks[HOOK_BEFORE][i])
                            ->Call(v8::Undefined(isolate->m_isolate),
                                0, NULL)
                            .IsEmpty()) {
                        coroutine_base::set_loglevel(oldlevel);
                        clear();
                        return 0;
                    }
            }

            if (p->m_pos < (int32_t)p->m_subs.size()) {
                exlib::string str(stack.size() * 2, ' ');

                p1 = p->m_subs[p->m_pos++];
                if (p->m_run_level > p1->m_level)
                    continue;

                if (p1->m_block.IsEmpty()) {
                    coroutine_base::set_loglevel(oldlevel);
                    if (stack.size() == 1)
                        asyncLog(console_base::_INFO, "");

                    str.append(logger::highLight());
                    str.append(p1->m_name);
                    str.append(COLOR_RESET);

                    asyncLog(console_base::_INFO, str);
                    coroutine_base::set_loglevel(loglevel);

                    stack.append(p1);
                    continue;
                }

                for (j = 0; j < (int32_t)stack.size(); j++) {
                    p2 = stack[j];
                    for (i = 0; i < (int32_t)p2->m_hooks[HOOK_BEFORECASE].size();
                         i++)
                        if (v8::Local<v8::Function>::New(isolate->m_isolate,
                                p2->m_hooks[HOOK_BEFORECASE][i])
                                ->Call(v8::Undefined(isolate->m_isolate),
                                    0, NULL)
                                .IsEmpty()) {
                            coroutine_base::set_loglevel(oldlevel);
                            clear();
                            return 0;
                        }
                }

                cnt++;
                {
                    TryCatch try_catch;
                    date_t d1, d2;

                    d1.now();
                    v8::Local<v8::Function>::New(isolate->m_isolate, p1->m_block)->Call(v8::Undefined(isolate->m_isolate), 0, NULL);
                    d2.now();

                    if (try_catch.HasCaught()) {
                        sprintf(buf, "%d) ", ++errcnt);

                        p1->m_error = true;
                        if (loglevel > console_base::_ERROR)
                            ReportException(try_catch, 0);
                        else if (loglevel == console_base::_ERROR) {
                            exlib::string str1(buf);

                            for (i = 1; i < (int32_t)stack.size(); i++) {
                                str1.append(stack[i]->m_name);
                                str1.append(" ", 1);
                            }
                            str1.append(p1->m_name);
                            names.append(logger::highLight() + str1 + COLOR_RESET);

                            msgs.append(GetException(try_catch, 0));
                        }

                        str.append(buf);
                        str.append(p1->m_name);
                    } else {
                        double n = d2.diff(d1);

                        str.append(logger::notice() + "\xe2\x88\x9a " COLOR_RESET);
                        str.append(p1->m_name);
                        if (n > s_slow / 2) {
                            sprintf(buf, " (%dms) ", (int32_t)n);

                            if (n > s_slow)
                                str.append(logger::error());
                            else
                                str.append(logger::warn());

                            str.append(buf);
                            str.append(COLOR_RESET);
                        }
                    }
                }

                coroutine_base::set_loglevel(oldlevel);
                asyncLog(
                    p1->m_error ? console_base::_ERROR : console_base::_INFO, str);
                coroutine_base::set_loglevel(loglevel);

                for (j = (int32_t)stack.size() - 1; j >= 0; j--) {
                    p2 = stack[j];
                    for (i = (int32_t)p2->m_hooks[HOOK_AFTERCASE].size() - 1;
                         i >= 0; i--)
                        if (v8::Local<v8::Function>::New(isolate->m_isolate,
                                p2->m_hooks[HOOK_AFTERCASE][i])
                                ->Call(v8::Undefined(isolate->m_isolate),
                                    0, NULL)
                                .IsEmpty()) {
                            coroutine_base::set_loglevel(oldlevel);
                            clear();
                            return 0;
                        }
                }
            }

            if (p->m_pos == (int32_t)p->m_subs.size()) {
                for (i = (int32_t)p->m_hooks[HOOK_AFTER].size() - 1; i >= 0; i--)
                    if (v8::Local<v8::Function>::New(isolate->m_isolate,
                            p->m_hooks[HOOK_AFTER][i])
                            ->Call(v8::Undefined(isolate->m_isolate),
                                0, NULL)
                            .IsEmpty()) {
                        coroutine_base::set_loglevel(oldlevel);
                        clear();
                        return 0;
                    }
                stack.pop();
            }
        }

        coroutine_base::set_loglevel(oldlevel);
        asyncLog(console_base::_INFO, "");

        da2.now();
        if (errcnt == 0) {
            sprintf(buf,
                (logger::notice() + "  \xe2\x88\x9a %d tests completed" COLOR_RESET " (%dms)").c_str(),
                cnt, (int32_t)da2.diff(da1));
            asyncLog(console_base::_INFO, buf);
        } else {
            sprintf(buf, (logger::error() + "  × %d of %d tests failed" COLOR_RESET " (%dms)").c_str(),
                errcnt, cnt, (int32_t)da2.diff(da1));
            asyncLog(console_base::_ERROR, buf);
        }

        asyncLog(console_base::_INFO, "");

        for (i = 0; i < (int32_t)msgs.size(); i++) {
            asyncLog(console_base::_INFO, names[i]);
            asyncLog(console_base::_ERROR, msgs[i]);
        }

        clear();

        retVal = errcnt;

        return 0;
    }

    static void clear()
    {
        TestData* td = TestData::current();

        td->m_root = NULL;
        td->m_now = NULL;
    }

private:
    exlib::string m_name;
    v8::Global<v8::Function> m_block;
    int32_t m_level;
    int32_t m_run_level;
    QuickArray<obj_ptr<_case>> m_subs;
    QuickArray<v8::Global<v8::Function>> m_hooks[4];
    int32_t m_pos;
    bool m_error;
};

inline bool checkCallback(const char* src)
{
    _parser p(src);
    exlib::string s;

    p.skipSpace();
    p.getWord(s, '(', '=', '.');
    if (!s.empty()) {
        if (s != "function")
            return true;

        p.skipSpace();
        p.getWord(s, '(');
        p.skipSpace();
    }

    p.skipSpace();
    if (p.getChar() != '(')
        return false;

    p.skipSpace();
    p.getWord(s, ')', ',');
    return !s.empty();
}

inline v8::Local<v8::Function> wrapFunction(v8::Local<v8::Function> func)
{
    if (func->IsAsyncFunction())
        util_base::sync(func, true, func);
    {
        v8::Local<v8::String> src = func->ToString();
        v8::String::Utf8Value tmp(src);
        if (checkCallback(*tmp))
            util_base::sync(func, false, func);
    }

    return func;
}

result_t test_base::describe(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, wrapFunction(block), _case::TEST_NORMAL);
}

result_t test_base::xdescribe(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, wrapFunction(block), _case::TEST_SKIP);
}

result_t test_base::odescribe(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, wrapFunction(block), _case::TEST_ONLY);
}

result_t test_base::it(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_NORMAL);
}

result_t test_base::xit(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_SKIP);
}

result_t test_base::oit(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_ONLY);
}

result_t test_base::before(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_BEFORE, wrapFunction(func));
}

result_t test_base::after(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_AFTER, wrapFunction(func));
}

result_t test_base::beforeEach(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_BEFORECASE, wrapFunction(func));
}

result_t test_base::afterEach(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_AFTERCASE, wrapFunction(func));
}

result_t test_base::run(int32_t loglevel, int32_t& retVal)
{
    return _case::run(loglevel, retVal);
}

result_t test_base::setup()
{
    Isolate* isolate = Isolate::current();

    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> glob = _context->Global();
    v8::Local<v8::Function> func, func1;

    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("assert"),
            assert_base::class_info().getModule(isolate))
        .IsJust();

    func = isolate->NewFunction("describe", s_describe);
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("describe"), func)
        .IsJust();

    func1 = isolate->NewFunction("xdescribe", s_xdescribe);
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("xdescribe"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewFromUtf8("skip"), func1)
        .IsJust();

    func1 = isolate->NewFunction("odescribe", s_odescribe);
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("odescribe"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewFromUtf8("only"), func1)
        .IsJust();

    func = isolate->NewFunction("it", s_it);
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("it"), func)
        .IsJust();

    func1 = isolate->NewFunction("xit", s_xit);
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("xit"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewFromUtf8("skip"), func1)
        .IsJust();

    func1 = isolate->NewFunction("oit", s_oit);
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("oit"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewFromUtf8("only"), func1)
        .IsJust();

    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("before"),
            isolate->NewFunction("before", s_before))
        .IsJust();
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("after"),
            isolate->NewFunction("after", s_after))
        .IsJust();
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("beforeEach"),
            isolate->NewFunction("beforeEach", s_beforeEach))
        .IsJust();
    glob->DefineOwnProperty(_context, isolate->NewFromUtf8("afterEach"),
            isolate->NewFunction("afterEach", s_afterEach))
        .IsJust();

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
