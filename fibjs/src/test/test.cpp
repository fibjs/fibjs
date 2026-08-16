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
#include "Fiber.h"
#include <stdlib.h>
#include <uv/include/uv.h>

namespace fibjs {

DECLARE_MODULE(test);

class _case;

static int32_t s_slow = 75;

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
        m_describe_block.Reset();
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
        p->m_describe_block.Reset(Isolate::current()->m_isolate, block);
        now->append(p);

        return 0;
    }

    bool expand_describe()
    {
        if (m_describe_block.IsEmpty())
            return true;

        // Skip describe block if marked as skipped (TEST_NONE)
        if (m_run_level == TEST_NONE) {
            m_describe_block.Reset();
            return true;
        }

        Isolate* isolate = Isolate::current();
        TestData* td = TestData::current();

        v8::Local<v8::Function> block = m_describe_block.Get(isolate->m_isolate);
        m_describe_block.Reset();

        _case* old_describe = td->m_describe;
        td->m_describe = this;

        TryCatch try_catch;
        v8::Local<v8::Value> result;
        bool success = block->Call(block->GetCreationContextChecked(),
                                v8::Object::New(isolate->m_isolate), 0, NULL)
                           .ToLocal(&result);

        // If a Promise is returned, wait for it to settle (async describe)
        if (success && !result.IsEmpty() && result->IsPromise())
            isolate->await(result);

        if (try_catch.HasCaught()) {
            // describe block threw: mark this suite failed, all its tests fail,
            // keep running (aligned with Node)
            m_status = false;
            m_before_failed = true;
            m_hook_error = "[describe] " + GetException(try_catch, 0, false, true);
            m_errors.append(m_hook_error);
        }

        td->m_describe = old_describe;

        return success;
    }

    static result_t it(exlib::string name, v8::Local<v8::Function> block, int32_t level)
    {
        TestData* td = TestData::current();

        _case::init();

        _case* now = td->m_describe;
        if (!now)
            now = td->m_running;
        if (!now)
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

        _case::init();

        _case* now = td->m_describe;
        if (!now)
            now = td->m_running;
        if (!now)
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
                // Expand describe block (deferred); a thrown describe block marks
                // this suite failed (m_before_failed), all its tests fail, keep running
                p->expand_describe();

                p->m_begin.now();
                p->m_retVal = v8::Object::New(isolate->m_isolate);
                p->m_retVal_tests = v8::Array::New(isolate->m_isolate);

                if (p->m_run_level != TEST_NONE) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    // Skip this suite's before hooks when a parent before/describe block failed
                    bool parent_failed = false;
                    for (j = 0; j < (int32_t)stack.size() - 1; j++)
                        if (stack[j]->m_before_failed) {
                            parent_failed = true;
                            break;
                        }

                    if (!parent_failed) {
                        for (i = 0; i < (int32_t)p->m_hooks[HOOK_BEFORE].size(); i++) {
                            TryCatch try_catch;
                            v8::Local<v8::Function> func = p->m_hooks[HOOK_BEFORE][i].Get(isolate->m_isolate);
                            func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                            if (try_catch.HasCaught()) {
                                // before failed: mark the suite failed, all its tests fail,
                                // keep running (aligned with Node)
                                p->m_before_failed = true;
                                p->m_hook_error = "[before] " + GetException(try_catch, 0, false, true);
                                p->m_errors.append(p->m_hook_error);
                                p->m_status = false;
                                break;
                            }
                        }
                    }
                }
            }

            if (p->m_pos < (int32_t)p->m_subs.size()) {
                exlib::string str(stack.size() * 2, ' ');

                p1 = p->m_subs[p->m_pos++];
                bool is_todo_case = (p1->m_level == _case::TEST_TODO);

                if (!p1->m_block.IsEmpty() || is_todo_case) {
                    // Parent before/describe block failed: this test fails directly
                    // without running (aligned with Node)
                    bool before_failed = false;
                    exlib::string before_err;
                    for (j = 0; j < (int32_t)stack.size(); j++) {
                        p2 = stack[j];
                        if (p2->m_before_failed) {
                            before_failed = true;
                            before_err = p2->m_hook_error;
                            break;
                        }
                    }

                    if (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO) {
                        v8::HandleScope handle_scope(isolate->m_isolate);

                        if (before_failed)
                            p1->m_errors.append(before_err);
                        else {
                            for (j = 0; j < (int32_t)stack.size(); j++) {
                                p2 = stack[j];
                                for (i = 0; i < (int32_t)p2->m_hooks[HOOK_BEFORECASE].size(); i++) {
                                    TryCatch try_catch;
                                    v8::Local<v8::Function> func = p2->m_hooks[HOOK_BEFORECASE][i].Get(isolate->m_isolate);
                                    func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                                    if (try_catch.HasCaught()) {
                                        // beforeEach failed: current test fails and its block
                                        // is skipped, subsequent tests keep running (aligned with Node)
                                        p1->m_errors.append("[beforeEach] " + GetException(try_catch, 0, false, true));
                                        break;
                                    }
                                }
                                if (p1->m_errors.size())
                                    break;
                            }
                        }
                    }

                    {
                        TryCatch try_catch;
                        date_t d1, d2;

                        d1.now();

                        if (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO && p1->m_errors.size() == 0) {
                            v8::HandleScope handle_scope(isolate->m_isolate);

                            td->m_running = p1;

                            v8::Local<v8::Function> func = p1->m_block.Get(isolate->m_isolate);
                            func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                            if (try_catch.HasCaught()) {
                                v8::Local<v8::Value> exp = try_catch.Exception();
                                if (exp->IsFunction()) {
                                    func = exp.As<v8::Function>();
                                    try_catch.Reset();
                                    func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                                }
                            }

                            if (try_catch.HasCaught()) {
                                p1->m_errors.append(GetException(try_catch, 0, false, true));
                            } else
                                for (int32_t i = 0; i < p1->m_evs.size(); i++)
                                    p1->m_evs[i]->ac_wait();

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
                                names.append(COLOR_BOLD + str1 + COLOR_RESET);

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
                                str.append(COLOR_GRAY + "\xe2\x97\x8b ");
                            } else {
                                p->m_pass++;
                                val->Set(_context, isolate->NewString("status"), isolate->NewString("passed")).IsJust();
                                str.append(logger::notice() + "\xe2\x88\x9a " + COLOR_RESET);
                            }

                            if (!p1->m_subs.size())
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
                        outLog(console_base::C_INFO, logger::error() + str + COLOR_RESET);
                    else if (mode > console_base::C_ERROR || p1->m_level >= p->m_run_level)
                        if (!p1->m_subs.size())
                            outLog(console_base::C_INFO, str);

                    if (p1->m_level >= p->m_run_level && p1->m_level != _case::TEST_TODO) {
                        v8::HandleScope handle_scope(isolate->m_isolate);

                        for (j = (int32_t)stack.size() - 1; j >= 0; j--) {
                            p2 = stack[j];
                            for (i = (int32_t)p2->m_hooks[HOOK_AFTERCASE].size() - 1; i >= 0; i--) {
                                TryCatch try_catch;
                                v8::Local<v8::Function> func = p2->m_hooks[HOOK_AFTERCASE][i].Get(isolate->m_isolate);
                                func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                                if (try_catch.HasCaught()) {
                                    // afterEach failed: mark current test failed (even if it
                                    // passed), keep running (aligned with Node)
                                    exlib::string err = "[afterEach] " + GetException(try_catch, 0, false, true);
                                    bool was_passed = p1->m_errors.size() == 0;

                                    p1->m_errors.append(err);
                                    if (was_passed) {
                                        p1->m_status = false;
                                        p->m_status = false;
                                        p->m_fail++;
                                        p->m_pass--;

                                        v8::Local<v8::Object> val = p->m_retVal_tests->Get(_context, p->m_pos - 1).ToLocalChecked().As<v8::Object>();
                                        val->Set(_context, isolate->NewString("status"), isolate->NewString("failed")).IsJust();
                                        val->Set(_context, isolate->NewString("trace"), isolate->NewString(err)).IsJust();

                                        outLog(console_base::C_ERROR, logger::error() + str + " [afterEach failed]" + COLOR_RESET);
                                    }
                                }
                            }
                        }
                    }
                }

                // 判断是否是 describe：有 describe_block 或已经展开的 subs，且没有 it 的 block
                if (p1->m_status && (p1->m_subs.size() || !p1->m_describe_block.IsEmpty() || (p1->m_block.IsEmpty() && p1->m_level != _case::TEST_TODO))) {
                    if (p1->m_level < p->m_run_level)
                        p1->m_run_level = TEST_NONE;

                    if (mode > console_base::C_ERROR || p1->m_run_level != TEST_NONE) {
                        str.append(COLOR_BOLD);
                        str.append(p1->m_title);
                        str.append(COLOR_RESET);

                        if (stack.size() == 1)
                            outLog(console_base::C_INFO, "");
                        outLog(console_base::C_INFO, str);
                    }

                    stack.append(p1);
                    continue;
                }
            }

            if (p->m_pos == (int32_t)p->m_subs.size()) {
                if (p->m_run_level != TEST_NONE) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    for (i = (int32_t)p->m_hooks[HOOK_AFTER].size() - 1; i >= 0; i--) {
                        TryCatch try_catch;
                        v8::Local<v8::Function> func = p->m_hooks[HOOK_AFTER][i].Get(isolate->m_isolate);
                        func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 0, NULL).IsEmpty();
                        if (try_catch.HasCaught()) {
                            // after failed: mark the suite failed, turn passed direct tests
                            // into failed, keep running (aligned with Node)
                            exlib::string err = "[after] " + GetException(try_catch, 0, false, true);

                            p->m_status = false;
                            p->m_errors.append(err);

                            v8::Local<v8::Array> tests = p->m_retVal_tests;
                            for (j = 0; j < (int32_t)tests->Length(); j++) {
                                v8::Local<v8::Value> v = tests->Get(_context, j).FromMaybe(v8::Local<v8::Value>());
                                if (v.IsEmpty() || !v->IsObject())
                                    continue;

                                v8::Local<v8::Object> o = v.As<v8::Object>();
                                // Skip sub-suite entries (result objects carrying a tests field)
                                if (o->Has(_context, isolate->NewString("tests")).FromMaybe(true))
                                    continue;

                                v8::Local<v8::Value> status = o->Get(_context, isolate->NewString("status")).FromMaybe(v8::Local<v8::Value>());
                                if (status.IsEmpty() || isolate->toString(status) != "passed")
                                    continue;

                                o->Set(_context, isolate->NewString("status"), isolate->NewString("failed")).IsJust();
                                o->Set(_context, isolate->NewString("trace"), isolate->NewString(err)).IsJust();
                                p->m_pass--;
                                p->m_fail++;
                            }

                            exlib::string errline(stack.size() * 2, ' ');
                            errline.append("[after failed] ");
                            errline.append(p->m_title);
                            outLog(console_base::C_ERROR, logger::error() + errline + COLOR_RESET);
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

        outLog(console_base::C_INFO, "");

        snprintf(buf, sizeof(buf),
            (COLOR_BOLD + "    %d tests completed" + COLOR_RESET + " (%dms)").c_str(),
            td->m_root->m_total, (int32_t)td->m_root->m_duration);
        outLog(console_base::C_INFO, buf);

        if (td->m_root->m_pass) {
            snprintf(buf, sizeof(buf),
                (logger::notice() + "  \xe2\x88\x9a %d tests passed" + COLOR_RESET).c_str(), td->m_root->m_pass);
            outLog(console_base::C_INFO, buf);
        }

        if (td->m_root->m_fail) {
            snprintf(buf, sizeof(buf), (logger::error() + "  \xc3\x97 %d tests failed" + COLOR_RESET).c_str(), td->m_root->m_fail);
            outLog(console_base::C_INFO, buf);
        }

        if (td->m_root->m_todo) {
            snprintf(buf, sizeof(buf), (COLOR_CYAN + "  \xe2\x98\x90 %d todo tests" + COLOR_RESET).c_str(), td->m_root->m_todo);
            outLog(console_base::C_INFO, buf);
        }

        if (td->m_root->m_skip) {
            snprintf(buf, sizeof(buf), (COLOR_GRAY + "  \xe2\x97\x8b %d tests skipped" + COLOR_RESET).c_str(), td->m_root->m_skip);
            outLog(console_base::C_INFO, buf);
        }

        outLog(console_base::C_INFO, "");

        for (i = 0; i < (int32_t)msgs.size(); i++) {
            outLog(console_base::C_INFO, names[i]);
            outLog(console_base::C_INFO, logger::error() + msgs[i] + COLOR_RESET);
        }

        retVal = td->m_root->m_retVal;

        clear();

        if (errcnt)
            process_base::set_exitCode(1);

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

    // Flag for a failed before/describe block: all its tests fail
    // (Node before-failure semantics)
    bool m_before_failed = false;
    exlib::string m_hook_error;

    date_t m_begin;
    double m_duration = 0;

    v8::Local<v8::Object> m_retVal;
    v8::Local<v8::Array> m_retVal_tests;

    v8::Global<v8::Function> m_block;
    v8::Global<v8::Function> m_describe_block;
    int32_t m_level;
    int32_t m_run_level;
    QuickArray<obj_ptr<_case>> m_subs;
    QuickArray<v8::Global<v8::Function>> m_hooks[4];
    int32_t m_pos = 0;
};

static void two_arg_test_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> func = args.Data().As<v8::Function>();

    int32_t len = args.Length();
    std::vector<v8::Local<v8::Value>> argv(len + 1);
    argv[0] = v8::Object::New(isolate->m_isolate);
    for (int32_t i = 0; i < len; i++)
        argv[i + 1] = args[i];

    v8::Local<v8::Value> result;
    if (func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).ToLocal(&result))
        args.GetReturnValue().Set(result);
}

inline v8::Local<v8::Function> wrapFunction(v8::Local<v8::Function> func)
{
    if (func->IsAsyncFunction())
        util_base::sync(func, true, func);
    {
        Isolate* isolate = Isolate::current();
        int32_t length = isolate->toInt32Value(JSValue(func->Get(isolate->context(), isolate->NewString("length"))));
        if (length >= 2) {
            // Node.js test style: (t, done) => {}
            // Create wrapper(done) that calls original(ctx, done)
            v8::Local<v8::Function> wrapper = isolate->NewFunction("test_wrapper", two_arg_test_wrapper, func);
            util_base::sync(wrapper, false, func);
        } else if (length > 0) {
            util_base::sync(func, false, func);
        }
    }

    return func;
}

static int32_t get_level_from_options(v8::Local<v8::Object> options, int32_t default_level)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> v;

    if (options->Get(context, isolate->NewString("skip")).ToLocal(&v) && v->IsTrue())
        return _case::TEST_SKIP;

    if (options->Get(context, isolate->NewString("todo")).ToLocal(&v) && v->IsTrue())
        return _case::TEST_TODO;

    if (options->Get(context, isolate->NewString("only")).ToLocal(&v) && v->IsTrue())
        return _case::TEST_ONLY;

    return default_level;
}

result_t test_base::_function(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_NORMAL);
}

result_t test_base::_function(exlib::string name, v8::Local<v8::Object> options, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), get_level_from_options(options, _case::TEST_NORMAL));
}

result_t test_suite_base::_function(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, block, _case::TEST_NORMAL);
}

result_t test_suite_base::_function(exlib::string name, v8::Local<v8::Object> options, v8::Local<v8::Function> block)
{
    return _case::describe(name, block, get_level_from_options(options, _case::TEST_NORMAL));
}

result_t test_suite_base::skip(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, block, _case::TEST_SKIP);
}

result_t test_suite_base::only(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, block, _case::TEST_ONLY);
}

result_t test_suite_base::todo(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::describe(name, block, _case::TEST_TODO);
}

result_t test_suite_base::todo(exlib::string name, v8::Local<v8::Object> options, v8::Local<v8::Function> block)
{
    return _case::describe(name, block, get_level_from_options(options, _case::TEST_TODO));
}

result_t test_base::xdescribe(exlib::string name, v8::Local<v8::Function> block)
{
    return test_suite_base::skip(name, block);
}

result_t test_base::odescribe(exlib::string name, v8::Local<v8::Function> block)
{
    return test_suite_base::only(name, block);
}

result_t test_base::skip(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_SKIP);
}

result_t test_base::xit(exlib::string name, v8::Local<v8::Function> block)
{
    return skip(name, block);
}

result_t test_base::only(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_ONLY);
}

result_t test_base::oit(exlib::string name, v8::Local<v8::Function> block)
{
    return only(name, block);
}

result_t test_base::todo(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), _case::TEST_TODO);
}

result_t test_base::todo(exlib::string name, v8::Local<v8::Object> options, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block), get_level_from_options(options, _case::TEST_TODO));
}

result_t test_base::todo(exlib::string name)
{
    return _case::it(name, v8::Local<v8::Function>(), _case::TEST_TODO);
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

static int32_t s_watchdog_ms = 10000;
static exlib::string s_watchdog_log;

extern exlib::atomic g_ExtStringCount;

// Recursively collect the nested structure from ClassInfo::dump:
// {class, objects, inherits:[...]}
static void collect_native_objects(v8::Local<v8::Value> v, v8::Local<v8::Context> ctx, Isolate* isolate,
    exlib::string& report)
{
    if (!v->IsObject())
        return;

    v8::Local<v8::Object> o = v.As<v8::Object>();
    v8::Local<v8::Value> cls = o->Get(ctx, isolate->NewString("class")).FromMaybe(v8::Local<v8::Value>());
    v8::Local<v8::Value> count = o->Get(ctx, isolate->NewString("objects")).FromMaybe(v8::Local<v8::Value>());

    if (!cls.IsEmpty() && !count.IsEmpty()) {
        exlib::string cls_name = isolate->toString(cls);

        report.append("[test-watchdog]   ");
        report.append(cls_name);
        report.append(": ");
        report.append(isolate->toString(count));
        report.append(1, '\n');
    }

    v8::Local<v8::Value> inherits = o->Get(ctx, isolate->NewString("inherits")).FromMaybe(v8::Local<v8::Value>());
    if (!inherits.IsEmpty() && inherits->IsArray()) {
        v8::Local<v8::Array> arr = inherits.As<v8::Array>();
        for (uint32_t i = 0; i < arr->Length(); i++)
            collect_native_objects(arr->Get(ctx, i).ToLocalChecked(), ctx, isolate, report);
    }
}

// Hang diagnostic report: fibers + stack, native object counts after GC, memory info
static void report_watchdog(Isolate* isolate)
{
    exlib::string report;
    char buf[256];

    report.append("=== fibjs test watchdog: process alive ");
    snprintf(buf, sizeof(buf), "%d", s_watchdog_ms);
    report.append(buf);
    report.append("ms after test run ended ===\n");

    // 1) Force GC: also drains the isolate's m_weak pending-delete list
    //    (fb_GCCallback), so nativeObjects reflects objects that are truly alive
    isolate->m_isolate->LowMemoryNotification();

    // 2) Alive fibers and their stacks
    int32_t fiber_count = 0;
    int64_t self_id = -1;
    JSFiber* self = JSFiber::current();
    if (self)
        self->get_id(self_id);

    exlib::linkitem* p = isolate->m_fibers.head();

    while (p) {
        JSFiber* fb = (JSFiber*)p;
        int64_t id;
        int32_t usage;
        exlib::string stack;

        fb->get_id(id);

        // Exclude the watchdog's own diagnostic fiber, report only leaked residual fibers
        if (id == self_id) {
            p = p->m_next;
            continue;
        }

        fb->get_stack_usage(usage);
        fb->get_stack(stack);

        report.append("[test-watchdog] fiber #");
        snprintf(buf, sizeof(buf), "%lld", (long long)id);
        report.append(buf);
        report.append(" (stack_usage=");
        snprintf(buf, sizeof(buf), "%d", usage);
        report.append(buf);
        report.append("):\n");

        size_t pos = 0;
        while (pos < stack.length()) {
            size_t nl = stack.find('\n', pos);

            report.append("[test-watchdog]   ");
            if (nl == exlib::string::npos) {
                report.append(stack.substr(pos, stack.length() - pos));
                pos = stack.length();
            } else {
                report.append(stack.substr(pos, nl - pos));
                pos = nl + 1;
            }
            report.append(1, '\n');
        }

        fiber_count++;
        p = p->m_next;
    }

    report.append("[test-watchdog] fibers: ");
    snprintf(buf, sizeof(buf), "%d", fiber_count);
    report.append(buf);
    report.append(1, '\n');

    // 3) Native object counts per class (after GC)
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> objs;

    report.append("[test-watchdog] nativeObjects (after gc):\n");
    object_base::class_info().dump(objs);
    if (!objs.IsEmpty())
        collect_native_objects(objs, _context, isolate, report);

    // 4) Memory info
    size_t rss = 0;
    uv_resident_set_memory(&rss);
    v8::HeapStatistics hs;
    isolate->m_isolate->GetHeapStatistics(&hs);

    report.append("[test-watchdog] memory: rss=");
    snprintf(buf, sizeof(buf), "%zu", rss);
    report.append(buf);
    report.append(" heapTotal=");
    snprintf(buf, sizeof(buf), "%zu", (size_t)hs.total_heap_size());
    report.append(buf);
    report.append(" heapUsed=");
    snprintf(buf, sizeof(buf), "%zu", (size_t)hs.used_heap_size());
    report.append(buf);
    report.append(" external=");
    snprintf(buf, sizeof(buf), "%zu", (size_t)hs.external_memory());
    report.append(buf);
    report.append(" ExtStrings=");
    snprintf(buf, sizeof(buf), "%d", (int32_t)g_ExtStringCount.value());
    report.append(buf);
    report.append(1, '\n');

    // Print to stderr (keep stdout clean for test results)
    fputs(report.c_str(), stderr);
    fflush(stderr);

    // Optional: append to a file (FIBJS_TEST_WATCHDOG_LOG)
    if (!s_watchdog_log.empty()) {
        FILE* f = fopen(s_watchdog_log.c_str(), "a");
        if (f) {
            fputs(report.c_str(), f);
            fclose(f);
        }
    }
}

// Watchdog timer task: not an object_base, so it never shows up in native object
// counts; when fired, dispatch to the JS thread to collect diagnostics and force
// exit (124)
class WatchdogTask : public exlib::Task_base {
public:
    WatchdogTask(Isolate* isolate)
        : m_isolate(isolate)
    {
    }

    virtual void resume()
    {
        m_isolate->sync([isolate = m_isolate]() -> int {
            v8::HandleScope handle_scope(isolate->m_isolate);
            JSFiber::EnterJsScope s;

            // Timer fired while the process is still alive => abnormal hang:
            // collect diagnostics and force exit (124)
            report_watchdog(isolate);
            process_base::exit(124);

            return 0;
        });

        delete this;
    }

private:
    Isolate* m_isolate;
};

// Arm the watchdog: the exlib timer task holds no isolate ref, so a normal exit
// (milliseconds) never triggers it
static void arm_test_watchdog(Isolate* isolate, int32_t ms)
{
    if (ms <= 0)
        return;

    exlib::Fiber::sleep(ms, new WatchdogTask(isolate));
}

void run_test(int32_t mode)
{
    TestData* td = TestData::current();
    if (!td->m_root)
        return;

    Isolate* isolate = Isolate::current();
    isolate->m_isolate->LowMemoryNotification();
    g_track_native_object = true;

    // Watchdog config: FIBJS_TEST_WATCHDOG_MS (milliseconds, 0 disables, default 10000)
    const char* env_ms = getenv("FIBJS_TEST_WATCHDOG_MS");
    if (env_ms) {
        int32_t ms = atoi(env_ms);
        if (ms < 0)
            ms = 0;
        s_watchdog_ms = ms;
    }

    // Also append the report to a file (optional)
    s_watchdog_log.clear();
    const char* env_log = getenv("FIBJS_TEST_WATCHDOG_LOG");
    if (env_log && *env_log)
        s_watchdog_log = env_log;

    isolate->sync([isolate, mode]() -> int {
        v8::HandleScope handle_scope(isolate->m_isolate);
        JSFiber::EnterJsScope s;

        v8::Local<v8::Object> ret;
        result_t hr = _case::run(mode, ret);
        if (hr < 0) {
            ThrowResult(hr);
        }

        // Arm the watchdog after the run: if the process does not exit within
        // s_watchdog_ms, treat it as an abnormal hang (resources not released),
        // collect diagnostics and force exit
        arm_test_watchdog(isolate, s_watchdog_ms);

        return 0;
    });
}

static void must_call(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> _context = isolate->context();

    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Function> func = _data->Get(_context, isolate->NewString("func")).FromMaybe(v8::Local<v8::Value>()).As<v8::Function>();
    obj_ptr<Event_base> ev = Event_base::getInstance(_data->Get(_context, isolate->NewString("ev")).FromMaybe(v8::Local<v8::Value>()));
    _case* running = (_case*)_data->Get(_context, isolate->NewString("case")).FromMaybe(v8::Local<v8::Value>()).As<v8::External>()->Value();

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
        td->m_running->m_errors.append(GetException(try_catch, 0, false, true));
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

    _case* running = (_case*)args.Data().As<v8::External>()->Value();
    TestData* td = TestData::current();
    if (td->m_running != running) {
        ThrowError("This function must be called in the same case.");
        return;
    }

    v8::Local<v8::Value> exp = FillError(CALL_E_EXCEPTION, "This function must never be called.");
    td->m_running->m_errors.append(GetException(exp, false, true));
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
