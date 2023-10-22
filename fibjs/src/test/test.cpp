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
#include "ifs/process.h"
#include "Event.h"
#include "QuickArray.h"
#include "StringBuffer.h"
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
    obj_ptr<_case> m_describe;
    _case* m_running = NULL;

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
    HOOK_AFTER,
    HOOK_BEFORECASE,
    HOOK_AFTERCASE
};

class _case : public obj_base {
    _case(exlib::string name = "", int32_t level = TEST_TODO)
        : m_level(level)
        , m_run_level(TEST_TODO)
    {
        m_title = name;
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
        TEST_TODO,
        TEST_NORMAL,
        TEST_ONLY,
        TEST_NONE
    };

public:
    void append(_case* c)
    {
        m_subs.append(c);
        if (c->m_level == TEST_ONLY)
            m_run_level = TEST_ONLY;
    }

public:
    static void init()
    {
        TestData* td = TestData::current();

        if (td->m_root == NULL)
            td->m_root = td->m_describe = new _case();
    }

    static result_t describe(exlib::string name, v8::Local<v8::Function> block, int32_t level)
    {
        TestData* td = TestData::current();

        _case::init();

        _case* now = td->m_describe;
        if (!now)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        _case* p = new _case(name, level);
        now->append(p);

        td->m_describe = p;
        block->Call(block->GetCreationContextChecked(), v8::Object::New(Isolate::current()->m_isolate), 0, NULL).IsEmpty();
        td->m_describe = now;

        return 0;
    }

    static result_t it(exlib::string name, v8::Local<v8::Function> block, int32_t level)
    {
        TestData* td = TestData::current();

        _case* now = td->m_describe;
        if (!now || !td->m_root)
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

        _case* now = td->m_describe;
        if (!td->m_describe)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        QuickArray<v8::Global<v8::Function>>& fa = now->m_hooks[type];
        size_t sz = fa.size();

        fa.resize(sz + 1);
        fa[sz].Reset(Isolate::current()->m_isolate, func);

        return 0;
    }

    static result_t run(int32_t mode, v8::Local<v8::Object>& retVal)
    {
        TestData* td = TestData::current();

        if (!td->m_root)
            return 0;

        if (td->m_describe != td->m_root)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        td->m_describe = NULL;

        QuickArray<obj_ptr<_case>> stack;
        QuickArray<exlib::string> names;
        QuickArray<exlib::string> msgs;
        int32_t i, j;
        int32_t errcnt = 0;
        char buf[128];

        stack.append(td->m_root);

        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> _context = isolate->context();

        while (stack.size()) {
            _case* p = stack[stack.size() - 1];
            _case *p1, *p2;

            if (p->m_pos == 0) {
                p->m_begin.now();
                p->m_retVal = v8::Object::New(isolate->m_isolate);
                p->m_retVal_tests = v8::Array::New(isolate->m_isolate);

                if (p->m_run_level != TEST_NONE) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    for (i = 0; i < (int32_t)p->m_hooks[HOOK_BEFORE].size(); i++) {
                        v8::Local<v8::Function> func = p->m_hooks[HOOK_BEFORE][i].Get(isolate->m_isolate);
                        if (func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty()) {
                            clear();
                            return 0;
                        }
                    }
                }
            }

            if (p->m_pos < (int32_t)p->m_subs.size()) {
                exlib::string str(stack.size() * 2, ' ');

                p1 = p->m_subs[p->m_pos++];

                if (p1->m_block.IsEmpty()) {
                    if (p1->m_level < p->m_run_level)
                        p1->m_run_level = TEST_NONE;

                    if (mode > console_base::C_ERROR || p1->m_run_level != TEST_NONE) {
                        str.append(logger::highLight());
                        str.append(p1->m_title);
                        str.append(COLOR_RESET);

                        if (stack.size() == 1)
                            asyncLog(console_base::C_INFO, "");
                        asyncLog(console_base::C_INFO, str);
                    }

                    stack.append(p1);
                    continue;
                }

                if (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    for (j = 0; j < (int32_t)stack.size(); j++) {
                        p2 = stack[j];
                        for (i = 0; i < (int32_t)p2->m_hooks[HOOK_BEFORECASE].size(); i++) {
                            v8::Local<v8::Function> func = p2->m_hooks[HOOK_BEFORECASE][i].Get(isolate->m_isolate);
                            if (func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty()) {
                                clear();
                                return 0;
                            }
                        }
                    }
                }

                {
                    TryCatch try_catch;
                    date_t d1, d2;

                    d1.now();

                    if (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO) {
                        v8::HandleScope handle_scope(isolate->m_isolate);

                        td->m_running = p1;

                        v8::Local<v8::Function> func = p1->m_block.Get(isolate->m_isolate);
                        func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                        if (try_catch.HasCaught()) {
                            v8::Local<v8::Value> exp = try_catch.Exception();
                            if (exp->IsFunction()) {
                                func = v8::Local<v8::Function>::Cast(exp);
                                try_catch.Reset();
                                func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                            }
                        }

                        if (try_catch.HasCaught()) {
                            p1->m_errors.append(GetException(try_catch, 0));
                        } else
                            for (int32_t i = 0; i < p1->m_evs.size(); i++)
                                p1->m_evs[i]->wait();

                        td->m_running = NULL;
                    }

                    d2.now();
                    p1->m_duration = d2.diff(d1);

                    v8::Local<v8::Object> val = v8::Object::New(isolate->m_isolate);
                    val->Set(_context, isolate->NewString("title"), isolate->NewString(p1->m_title)).IsJust();

                    p->m_total++;
                    if (p1->m_errors.size()) {
                        exlib::string err_msg = p1->m_errors.str();

                        p->m_fail++;
                        snprintf(buf, sizeof(buf), "%d) ", ++errcnt);

                        p1->m_status = false;
                        p->m_status = false;

                        if (mode > console_base::C_ERROR)
                            errorLog(err_msg);
                        else if (mode == console_base::C_ERROR) {
                            exlib::string str1(buf);

                            for (i = 1; i < (int32_t)stack.size(); i++) {
                                str1.append(stack[i]->m_title);
                                str1.append(" ", 1);
                            }
                            str1.append(p1->m_title);
                            names.append(logger::highLight() + str1 + COLOR_RESET);

                            msgs.append(err_msg);
                        }

                        val->Set(_context, isolate->NewString("status"), isolate->NewString("failed")).IsJust();
                        val->Set(_context, isolate->NewString("trace"), isolate->NewString(err_msg)).IsJust();

                        str.append(buf);
                        str.append(p1->m_title);
                    } else {
                        if (p1->m_level == _case::TEST_TODO) {
                            p->m_todo++;
                            val->Set(_context, isolate->NewString("status"), isolate->NewString("todo")).IsJust();
                            str.append(COLOR_CYAN + "\xe2\x98\x90 ");
                        } else if (p1->m_level < p->m_run_level) {
                            p->m_skip++;
                            val->Set(_context, isolate->NewString("status"), isolate->NewString("skipped")).IsJust();
                            str.append(COLOR_GREY + "\xe2\x97\x8b ");
                        } else {
                            p->m_pass++;
                            val->Set(_context, isolate->NewString("status"), isolate->NewString("passed")).IsJust();
                            str.append(logger::notice() + "\xe2\x88\x9a " + COLOR_RESET);
                        }

                        str.append(p1->m_title);
                        if (p1->m_duration > s_slow / 2) {
                            snprintf(buf, sizeof(buf), " (%dms) ", (int32_t)p1->m_duration);

                            if (p1->m_duration > s_slow)
                                str.append(logger::error());
                            else
                                str.append(logger::warn());

                            str.append(buf);
                            str.append(COLOR_RESET);
                        }
                    }

                    val->Set(_context, isolate->NewString("duration"), v8::Number::New(isolate->m_isolate, p1->m_duration)).IsJust();

                    p->m_retVal_tests->Set(_context, p->m_pos - 1, val).IsJust();
                }

                if (!p1->m_status)
                    asyncLog(console_base::C_INFO, logger::error() + str + COLOR_RESET);
                else if (mode > console_base::C_ERROR || (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO))
                    asyncLog(console_base::C_INFO, str);

                if (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    for (j = (int32_t)stack.size() - 1; j >= 0; j--) {
                        p2 = stack[j];
                        for (i = (int32_t)p2->m_hooks[HOOK_AFTERCASE].size() - 1; i >= 0; i--) {
                            v8::Local<v8::Function> func = p2->m_hooks[HOOK_AFTERCASE][i].Get(isolate->m_isolate);
                            if (func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty()) {
                                clear();
                                return 0;
                            }
                        }
                    }
                }
            }

            if (p->m_pos == (int32_t)p->m_subs.size()) {
                if (p->m_run_level != TEST_NONE) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    for (i = (int32_t)p->m_hooks[HOOK_AFTER].size() - 1; i >= 0; i--) {
                        v8::Local<v8::Function> func = p->m_hooks[HOOK_AFTER][i].Get(isolate->m_isolate);
                        if (func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty()) {
                            clear();
                            return 0;
                        }
                    }
                }

                date_t d2;

                d2.now();
                p->m_duration = d2.diff(p->m_begin);

                if (stack.size() > 1)
                    p->m_retVal->Set(_context, isolate->NewString("title"), isolate->NewString(p->m_title)).IsJust();

                p->m_retVal->Set(_context, isolate->NewString("status"), p->m_status ? isolate->NewString("passed") : isolate->NewString("failed")).IsJust();

                p->m_retVal->Set(_context, isolate->NewString("total"), v8::Number::New(isolate->m_isolate, p->m_total)).IsJust();

                if (p->m_pass)
                    p->m_retVal->Set(_context, isolate->NewString("passed"), v8::Number::New(isolate->m_isolate, p->m_pass)).IsJust();
                if (p->m_fail)
                    p->m_retVal->Set(_context, isolate->NewString("failed"), v8::Number::New(isolate->m_isolate, p->m_fail)).IsJust();
                if (p->m_todo)
                    p->m_retVal->Set(_context, isolate->NewString("todo"), v8::Number::New(isolate->m_isolate, p->m_todo)).IsJust();
                if (p->m_skip)
                    p->m_retVal->Set(_context, isolate->NewString("skipped"), v8::Number::New(isolate->m_isolate, p->m_skip)).IsJust();

                p->m_retVal->Set(_context, isolate->NewString("duration"), v8::Number::New(isolate->m_isolate, p->m_duration)).IsJust();

                p->m_retVal->Set(_context, isolate->NewString("tests"), p->m_retVal_tests).IsJust();

                stack.pop();

                if (stack.size() > 0) {
                    p1 = stack[stack.size() - 1];

                    p1->m_retVal_tests->Set(_context, p1->m_pos - 1, p->m_retVal).IsJust();

                    p1->m_total += p->m_total;
                    p1->m_pass += p->m_pass;
                    p1->m_fail += p->m_fail;
                    p1->m_skip += p->m_skip;
                    p1->m_todo += p->m_todo;
                    if (!p->m_status)
                        p1->m_status = false;
                }
            }
        }

        asyncLog(console_base::C_INFO, "");

        snprintf(buf, sizeof(buf),
            (logger::highLight() + "    %d tests completed" + COLOR_RESET + " (%dms)").c_str(),
            td->m_root->m_total, (int32_t)td->m_root->m_duration);
        asyncLog(console_base::C_INFO, buf);

        if (td->m_root->m_pass) {
            snprintf(buf, sizeof(buf),
                (logger::notice() + "  \xe2\x88\x9a %d tests passed" + COLOR_RESET).c_str(), td->m_root->m_pass);
            asyncLog(console_base::C_INFO, buf);
        }

        if (td->m_root->m_fail) {
            snprintf(buf, sizeof(buf), (logger::error() + "  \xc3\x97 %d tests failed" + COLOR_RESET).c_str(), td->m_root->m_fail);
            asyncLog(console_base::C_INFO, buf);
        }

        if (td->m_root->m_todo) {
            snprintf(buf, sizeof(buf), (COLOR_CYAN + "  \xe2\x98\x90 %d todo tests" + COLOR_RESET).c_str(), td->m_root->m_todo);
            asyncLog(console_base::C_INFO, buf);
        }

        if (td->m_root->m_skip) {
            snprintf(buf, sizeof(buf), (COLOR_GREY + "  \xe2\x97\x8b %d tests skipped" + COLOR_RESET).c_str(), td->m_root->m_skip);
            asyncLog(console_base::C_INFO, buf);
        }

        asyncLog(console_base::C_INFO, "");

        for (i = 0; i < (int32_t)msgs.size(); i++) {
            asyncLog(console_base::C_INFO, names[i]);
            asyncLog(console_base::C_INFO, logger::error() + msgs[i] + COLOR_RESET);
        }

        retVal = td->m_root->m_retVal;

        clear();

        if (errcnt)
            process_base::set_exitCode(errcnt);

        return 0;
    }

    static void clear()
    {
        TestData* td = TestData::current();

        td->m_root = NULL;
        td->m_describe = NULL;
    }

public:
    QuickArray<obj_ptr<Event_base>> m_evs;
    StringBuffer m_errors;

private:
    exlib::string m_title;
    int32_t m_total = 0;
    int32_t m_pass = 0;
    int32_t m_fail = 0;
    int32_t m_skip = 0;
    int32_t m_todo = 0;

    bool m_status = true;

    date_t m_begin;
    double m_duration = 0;

    v8::Local<v8::Object> m_retVal;
    v8::Local<v8::Array> m_retVal_tests;

    v8::Global<v8::Function> m_block;
    int32_t m_level;
    int32_t m_run_level;
    QuickArray<obj_ptr<_case>> m_subs;
    QuickArray<v8::Global<v8::Function>> m_hooks[4];
    int32_t m_pos = 0;
};

inline v8::Local<v8::Function> wrapFunction(v8::Local<v8::Function> func)
{
    if (func->IsAsyncFunction())
        util_base::sync(func, true, func);
    {
        Isolate* isolate = Isolate::current();
        if (isolate->toInt32Value(JSValue(func->Get(isolate->context(), isolate->NewString("length")))) > 0)
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

result_t test_base::todo(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_TODO);
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

result_t test_base::run(int32_t mode, v8::Local<v8::Object>& retVal)
{
    return _case::run(mode, retVal);
}

static void must_call(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> _context = isolate->context();

    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());
    v8::Local<v8::Function> func = _data->Get(_context, isolate->NewString("func")).FromMaybe(v8::Local<v8::Value>()).As<v8::Function>();
    obj_ptr<Event_base> ev = Event_base::getInstance(_data->Get(_context, isolate->NewString("ev")).FromMaybe(v8::Local<v8::Value>()));
    _case* running = (_case*)v8::Local<v8::External>::Cast(_data->Get(_context, isolate->NewString("case")).FromMaybe(v8::Local<v8::Value>()).As<v8::External>())->Value();

    TestData* td = TestData::current();
    if (td->m_running != running) {
        ThrowError("This function must be called in the same case.");
        return;
    }

    std::vector<v8::Local<v8::Value>> argv;
    argv.resize(args.Length());
    for (int32_t i = 0; i < args.Length(); i++)
        argv[i] = args[i];

    TryCatch try_catch;

    v8::Local<v8::Value> result;
    func->Call(func->GetCreationContextChecked(), args.This(), argv.size(), argv.data()).ToLocal(&result);
    args.GetReturnValue().Set(result);

    if (try_catch.HasCaught()) {
        td->m_running->m_errors.append(GetException(try_catch, 0));
        try_catch.ReThrow();
    }

    ev->set();
}

result_t test_base::mustCall(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    TestData* td = TestData::current();
    if (!td->m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);
    obj_ptr<Event_base> ev = new Event();

    td->m_running->m_evs.append(ev);

    _data->Set(_context, isolate->NewString("func"), func).IsJust();
    _data->Set(_context, isolate->NewString("ev"), ev->wrap()).IsJust();
    _data->Set(_context, isolate->NewString("case"), v8::External::New(isolate->m_isolate, td->m_running)).IsJust();

    retVal = isolate->NewFunction("mustNotCall", must_call, _data);

    return 0;
}

static void not_call(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> _context = isolate->context();

    _case* running = (_case*)v8::Local<v8::External>::Cast(args.Data().As<v8::External>())->Value();
    TestData* td = TestData::current();
    if (td->m_running != running) {
        ThrowError("This function must be called in the same case.");
        return;
    }

    v8::Local<v8::Value> exp = FillError(CALL_E_EXCEPTION, "This function must never be called.");
    td->m_running->m_errors.append(GetException(exp));
    ThrowError(exp);
}

result_t test_base::mustNotCall(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    TestData* td = TestData::current();
    if (!td->m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    Isolate* isolate = Isolate::current();
    retVal = isolate->NewFunction("mustNotCall", not_call, v8::External::New(isolate->m_isolate, td->m_running));
    return 0;
}

result_t test_base::mustNotCall(v8::Local<v8::Function>& retVal)
{
    TestData* td = TestData::current();
    if (!td->m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    Isolate* isolate = Isolate::current();
    retVal = isolate->NewFunction("mustNotCall", not_call, v8::External::New(isolate->m_isolate, td->m_running));
    return 0;
}

result_t test_base::setup()
{
    Isolate* isolate = Isolate::current();

    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> glob = _context->Global();
    v8::Local<v8::Function> func, func1;

    isolate->m_isolate->LowMemoryNotification();
    g_track_native_object = true;

    glob->DefineOwnProperty(_context, isolate->NewString("assert"),
            assert_base::class_info().getModule(isolate))
        .IsJust();

    func = isolate->NewFunction("describe", s_static_describe);
    glob->DefineOwnProperty(_context, isolate->NewString("describe"), func)
        .IsJust();

    func1 = isolate->NewFunction("xdescribe", s_static_xdescribe);
    glob->DefineOwnProperty(_context, isolate->NewString("xdescribe"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewString("skip"), func1)
        .IsJust();

    func1 = isolate->NewFunction("odescribe", s_static_odescribe);
    glob->DefineOwnProperty(_context, isolate->NewString("odescribe"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewString("only"), func1)
        .IsJust();

    func = isolate->NewFunction("it", s_static_it);
    glob->DefineOwnProperty(_context, isolate->NewString("it"), func)
        .IsJust();

    func1 = isolate->NewFunction("xit", s_static_xit);
    glob->DefineOwnProperty(_context, isolate->NewString("xit"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewString("skip"), func1)
        .IsJust();

    func1 = isolate->NewFunction("oit", s_static_oit);
    glob->DefineOwnProperty(_context, isolate->NewString("oit"), func1)
        .IsJust();
    func->DefineOwnProperty(_context, isolate->NewString("only"), func1)
        .IsJust();

    glob->DefineOwnProperty(_context, isolate->NewString("todo"),
            isolate->NewFunction("todo", s_static_todo))
        .IsJust();

    glob->DefineOwnProperty(_context, isolate->NewString("before"),
            isolate->NewFunction("before", s_static_before))
        .IsJust();
    glob->DefineOwnProperty(_context, isolate->NewString("after"),
            isolate->NewFunction("after", s_static_after))
        .IsJust();
    glob->DefineOwnProperty(_context, isolate->NewString("beforeEach"),
            isolate->NewFunction("beforeEach", s_static_beforeEach))
        .IsJust();
    glob->DefineOwnProperty(_context, isolate->NewString("afterEach"),
            isolate->NewFunction("afterEach", s_static_afterEach))
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
