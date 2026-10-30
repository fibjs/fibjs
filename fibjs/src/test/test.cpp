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

// Kill all child processes still running, return the number signalled.
// Defined in process/ChildProcess.cpp: declared here instead of including
// ChildProcess.h, which #undef's stdout/stderr (a trap for any C-runtime
// output added to this file).
int32_t child_process_kill_alive_children();

class _case;

static void warn_legacy_callback(v8::Local<v8::Function> func, const exlib::string& name);

// Set once the test tree has finished running (the summary has been printed).
// From that point on the process is draining the event loop, so any exception
// that escapes a JS scope is asynchronous activity no test can observe — Node
// reports the same situation as "generated asynchronous activity after the
// test ended" and fails the run.
static bool s_test_run_finished = false;

static int32_t s_slow = 75;

class TestData {
public:
    obj_ptr<_case> m_root;
    obj_ptr<_case> m_describe;
    _case* m_running = NULL;

    // Compiled RegExp[] from --test-name-pattern / --test-skip-pattern
    // (opt_tools/test.js stores them on the global before loading test files;
    // read once at run_test time)
    QuickArray<v8::Global<v8::RegExp>> m_name_patterns;
    QuickArray<v8::Global<v8::RegExp>> m_skip_patterns;

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

// The object handed to a test body (Node's TestContext) and to a suite body or
// hook (SuiteContext). See make_case_context() for the members.
enum {
    CTX_TEST = 0,
    CTX_SUITE
};

static v8::Local<v8::Object> make_case_context(_case* c, int32_t kind);

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

    // Small read/write surface for the context object handed to a body.
    void request_skip() { m_runtime_skip = true; }
    void request_todo() { m_runtime_todo = true; }

    exlib::string context_name() { return m_title; }
    exlib::string context_full_name() { return m_display_name.empty() ? m_title : m_display_name; }
    bool context_passed() { return m_status; }

    void add_diagnostic(exlib::string msg) { m_diagnostics.append(msg); }

public:
    void append(_case* c)
    {
        // Node's getTestNameWithAncestors(): ancestor titles joined by single
        // spaces, root excluded. Used as the second match target so a test can
        // be addressed unambiguously as 'suite sub test'.
        c->m_full_name = m_full_name.empty() ? c->m_title : m_full_name + " " + c->m_title;

        // Same chain in Node's display form ('suite > sub > case'), used by
        // context.fullName; the space-joined one above is what patterns match.
        if (m_is_suite || !m_display_name.empty()) {
            exlib::string prefix = m_display_name.empty() ? m_title : m_display_name;
            c->m_display_name = prefix + " > " + c->m_title;
        } else
            c->m_display_name = c->m_title;

        // todo is inherited from the enclosing suite (Node: isTodo = ... ||
        // parent.isTodo), so a describe.todo body's cases count as todo too.
        // The implicit root is also constructed with TEST_TODO, hence the
        // m_is_suite guard: only an explicitly registered suite inherits.
        if (m_todo_inherited || (m_is_suite && m_level == TEST_TODO) || c->m_level == TEST_TODO)
            c->m_todo_inherited = true;

        m_subs.append(c);
        if (c->m_level == TEST_ONLY)
            m_run_level = TEST_ONLY;
    }

public:
    static void init()
    {
        TestData* td = TestData::current();

        if (td->m_root == NULL) {
            // Node names the implicit root '<root>' and testMatchesPattern
            // recurses into it, so a pattern matching that name (or hitting it
            // through the ancestor-joined full name) exposes every case.
            td->m_root = td->m_describe = new _case("<root>");
            td->m_root->m_full_name = "";
        }
    }

    static result_t describe(exlib::string name, v8::Local<v8::Function> block, int32_t level)
    {
        TestData* td = TestData::current();

        _case::init();

        _case* now = td->m_describe;
        if (!now)
            return result_t::invalid_call();

        _case* p = new _case(name, level);
        p->m_is_suite = true;
        p->m_describe_block.Reset(Isolate::current()->m_isolate, block);
        now->append(p);

        return result_t();
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
        v8::Local<v8::Value> ctx_arg = make_case_context(this, CTX_SUITE);
        bool success = block->Call(block->GetCreationContextChecked(),
                                ctx_arg, 1, &ctx_arg)
                           .ToLocal(&result);

        // If a Promise is returned, wait for it to settle (async describe)
        if (success && !result.IsEmpty() && result->IsPromise())
            isolate->await(result);

        if (try_catch.HasCaught()) {
            // describe block threw: mark this suite failed, all its tests fail,
            // keep running (aligned with Node)
            m_status = false;
            m_before_failed = true;
            m_hook_error = "[describe] " + GetException(try_catch, result_t(), false, true);
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
            return result_t::invalid_call();

        _case* p = new _case(name, level);
        p->m_block.Reset(Isolate::current()->m_isolate, block);

        now->append(p);
        return result_t();
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
            return result_t::invalid_call();

        QuickArray<v8::Global<v8::Function>>& fa = now->m_hooks[type];
        size_t sz = fa.size();

        fa.resize(sz + 1);
        fa[sz].Reset(Isolate::current()->m_isolate, func);

        return result_t();
    }

    static bool matches_any(QuickArray<v8::Global<v8::RegExp>>& pats, const exlib::string& name)
    {
        if (!pats.size())
            return false;

        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::String> str = isolate->NewString(name);

        for (int32_t i = 0; i < (int32_t)pats.size(); i++) {
            v8::Local<v8::RegExp> re = pats[i].Get(isolate->m_isolate);
            v8::Local<v8::Object> v = re->Exec(context, str).FromMaybe(v8::Local<v8::Object>());
            if (!v.IsEmpty() && v->IsArray() && v.As<v8::Array>()->Length() > 0)
                return true;
        }

        return false;
    }

    // Node's testMatchesPattern minus the parent recursion. A pattern matches
    // either the node's own title or the ancestor titles joined by single
    // spaces (getTestNameWithAncestors()); the parent recursion is supplied by
    // m_chain_matched / m_chain_skipped, which propagate down the tree.
    static bool matches_node(TestData* td, _case* p, bool for_skip)
    {
        QuickArray<v8::Global<v8::RegExp>>& pats = for_skip ? td->m_skip_patterns : td->m_name_patterns;

        if (!pats.size())
            return false;

        if (matches_any(pats, p->m_title))
            return true;

        return p->m_full_name != p->m_title && matches_any(pats, p->m_full_name);
    }

    // Chain state of one child: true when the child itself matches or an
    // ancestor already did (Node's recursive testMatchesPattern).
    static bool chain_state(TestData* td, _case* parent, _case* c, bool for_skip)
    {
        bool inherited = for_skip ? parent->m_chain_skipped : parent->m_chain_matched;

        return inherited || matches_node(td, c, for_skip);
    }

    // Visibility of a node whose chain flags are up to date: a name pattern
    // must match somewhere on the chain, a skip pattern must match nowhere on
    // it. No filters means everything is visible.
    static bool node_visible(TestData* td, _case* c)
    {
        if (td->m_name_patterns.size() && !c->m_chain_matched)
            return false;
        if (td->m_skip_patterns.size() && c->m_chain_skipped)
            return false;

        return true;
    }

    // Whether any test below this suite stays visible. Describe blocks are
    // expanded on the way (expand_describe is idempotent: it clears the block
    // before invoking the callback), mirroring Node, where suite callbacks
    // always build but hooks and tests only run for visible content.
    //
    // A suite whose own title does not match is still visible when a distant
    // descendant matches the ancestor-joined full name (Node computes
    // filteredByName per node and then clears the ancestors), so the walk keeps
    // descending as long as the skip pattern has not pruned the subtree.
    static bool suite_visible(TestData* td, _case* p)
    {
        if (!p->m_describe_block.IsEmpty())
            p->expand_describe();

        for (int32_t i = 0; i < (int32_t)p->m_subs.size(); i++) {
            _case* s = p->m_subs[i];

            s->m_chain_matched = s->m_chain_matched || chain_state(td, p, s, false);
            s->m_chain_skipped = s->m_chain_skipped || chain_state(td, p, s, true);

            if (td->m_skip_patterns.size() && s->m_chain_skipped)
                continue; // skip pattern pruned this subtree: nothing below shows

            if (s->m_chain_matched)
                return true; // title/full-name hit: the container stays visible
                             // (its hooks run) even when the matching case is
                             // marked skip/todo and therefore never executes

            if (s->m_level == _case::TEST_SKIP)
                continue; // skipped subtrees never expand/run
            if (s->m_level < p->m_run_level)
                continue; // below the run level (only-marked parents): the
                          // counted-skip path is invisible while filtering

            if (s->m_is_suite && suite_visible(td, s))
                return true; // a descendant may still match by full name
        }

        return false;
    }

    static result_t run(int32_t mode, v8::Local<v8::Object>& retVal)
    {
        TestData* td = TestData::current();

        if (!td->m_root)
            return result_t();

        if (td->m_describe != td->m_root)
            return result_t::invalid_call();

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

        // Seed the root's chain state: Node's match function walks up to the
        // root, so '<root>' matching a pattern makes everything visible.
        td->m_root->m_chain_matched = matches_node(td, td->m_root, false);
        td->m_root->m_chain_skipped = matches_node(td, td->m_root, true);

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
                            v8::Local<v8::Value> ctx_arg = make_case_context(p, CTX_SUITE);
                            func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 1, &ctx_arg).IsEmpty();
                            if (try_catch.HasCaught()) {
                                // before failed: mark the suite failed, all its tests fail,
                                // keep running (aligned with Node). The failure itself
                                // goes through the numbered channel so the run exits 1.
                                p->m_before_failed = true;
                                p->m_hook_error = "[before] " + GetException(try_catch, result_t(), false, true);
                                p->m_errors.append(p->m_hook_error);
                                p->m_status = false;

                                if (mode <= console_base::C_ERROR) {
                                    exlib::string str1;

                                    snprintf(buf, sizeof(buf), "%d) ", ++errcnt);
                                    str1 = buf;
                                    // stack.back() is this suite (the stack has
                                    // not been popped yet), so the title only
                                    // carries the ancestors before it.
                                    for (j = 1; j < (int32_t)stack.size() - 1; j++) {
                                        str1.append(stack[j]->m_title);
                                        str1.append(" ", 1);
                                    }
                                    str1.append(p->m_title);
                                    str1.append(" (before hook)");
                                    names.append(COLOR_BOLD + str1 + COLOR_RESET);
                                    msgs.append(p->m_hook_error);
                                } else
                                    errorLog(p->m_hook_error);

                                break;
                            }
                        }
                    }
                }
            }

            if (p->m_pos < (int32_t)p->m_subs.size()) {
                exlib::string str(stack.size() * 2, ' ');

                p1 = p->m_subs[p->m_pos++];

                // A suite stays a suite after expand_describe() cleared its
                // block; a bodyless todo(...) is a leaf case (Node counts it).
                bool is_suite = p1->m_is_suite;
                bool is_leaf = !is_suite && (!p1->m_block.IsEmpty() || p1->m_level == _case::TEST_TODO);
                // skip wins over todo, as in Node's counters (pass() marks both,
                // countCompletedTest checks skipped first).
                bool is_todo = (p1->m_level != _case::TEST_SKIP)
                    && (p1->m_level == _case::TEST_TODO || p1->m_todo_inherited);

                // --test-name-pattern / --test-skip-pattern (chain rule, Node
                // parity): a node is visible when a name pattern matches its
                // own title, the ancestor-joined full name or an ancestor
                // title, and no skip pattern does. Invisible nodes are neither
                // executed nor counted nor printed (explicit skip/todo marks
                // included, like Node).
                p1->m_chain_matched = p1->m_chain_matched || chain_state(td, p, p1, false);
                p1->m_chain_skipped = p1->m_chain_skipped || chain_state(td, p, p1, true);

                bool filtering = td->m_name_patterns.size() || td->m_skip_patterns.size();

                if (filtering && !node_visible(td, p1)) {
                    if (is_leaf)
                        continue; // pruned: not executed, not counted
                    if (p1->m_level == _case::TEST_SKIP)
                        continue; // skipped suites never expand
                    // A suite that does not match is kept only when something
                    // below stays visible; suite_visible() expands the describe
                    // blocks on the way, so its side effects still happen.
                    if (!suite_visible(td, p1))
                        continue;
                }

                // Below the run level (a sibling marked `.only`): without a
                // filter the entry is counted as skipped; while filtering it is
                // invisible, like Node. Only a plain case can be shadowed this
                // way — skip/todo marks have their own levels.
                if (filtering && is_leaf && p1->m_level == _case::TEST_NORMAL
                    && p1->m_level < p->m_run_level)
                    continue;

                if (is_leaf) {
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

                    // Node counts the cases of a suite whose before hook threw
                    // as cancelled (they never start) rather than as failures.
                    p1->m_cancelled = before_failed;

                    if (p1->m_level >= p->m_run_level) {
                        v8::HandleScope handle_scope(isolate->m_isolate);

                        if (before_failed)
                            p1->m_errors.append(before_err);
                        else {
                            for (j = 0; j < (int32_t)stack.size(); j++) {
                                p2 = stack[j];
                                for (i = 0; i < (int32_t)p2->m_hooks[HOOK_BEFORECASE].size(); i++) {
                                    TryCatch try_catch;
                                    v8::Local<v8::Function> func = p2->m_hooks[HOOK_BEFORECASE][i].Get(isolate->m_isolate);
                                    v8::Local<v8::Value> ctx_arg = make_case_context(p1, CTX_TEST);
                                    func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 1, &ctx_arg)
                                        .IsEmpty();
                                    if (try_catch.HasCaught()) {
                                        // beforeEach failed: current test fails and its block
                                        // is skipped, subsequent tests keep running (aligned with Node)
                                        p1->m_errors.append("[beforeEach] " + GetException(try_catch, result_t(), false, true));
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

                        // A todo case still runs its body and per-case hooks
                        // (Node parity); the result is classified as todo below,
                        // so a throwing body only shows up in the details and
                        // never fails the run. A bodyless todo(...) just has no
                        // body to run.
                        if (p1->m_level >= p->m_run_level && p1->m_errors.size() == 0 && !p1->m_block.IsEmpty()) {
                            v8::HandleScope handle_scope(isolate->m_isolate);

                            td->m_running = p1;

                            v8::Local<v8::Function> func = p1->m_block.Get(isolate->m_isolate);
                            v8::Local<v8::Value> ctx_arg = make_case_context(p1, CTX_TEST);
                            func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 1, &ctx_arg).IsEmpty();
                            if (try_catch.HasCaught()) {
                                v8::Local<v8::Value> exp = try_catch.Exception();
                                if (exp->IsFunction()) {
                                    // legacy: throwing a function from the body used to
                                    // mean "run this as the body" (kept for compatibility)
                                    func = exp.As<v8::Function>();
                                    try_catch.Reset();
                                    func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 1, &ctx_arg).IsEmpty();
                                }
                            }

                            if (try_catch.HasCaught()) {
                                p1->m_errors.append(GetException(try_catch, result_t(), false, true));
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
                        if (is_todo || p1->m_runtime_todo) {
                            // todo (explicit, inherited from a describe.todo or
                            // requested by ctx.todo()): counted as todo, never as
                            // a failure. A throwing body is still reported on the
                            // case line, but it does not fail the run (Node keeps
                            // exit 0).
                            p->m_todo++;
                            val->Set(_context, isolate->NewString("status"), isolate->NewString("todo")).IsJust();
                            str.append(COLOR_CYAN + "\xe2\x98\x90 ");
                            str.append(p1->m_title);

                            if (p1->m_errors.size()) {
                                exlib::string err_msg = p1->m_errors.str();
                                if (mode > console_base::C_ERROR)
                                    errorLog(err_msg);
                                val->Set(_context, isolate->NewString("trace"), isolate->NewString(err_msg)).IsJust();
                            }
                        } else if (p1->m_cancelled) {
                            // An ancestor's before hook threw: the case never
                            // started, so Node counts it as cancelled rather than
                            // failed (the suite itself carries the failure).
                            p->m_cancel++;
                            val->Set(_context, isolate->NewString("status"), isolate->NewString("cancelled")).IsJust();
                            str.append(COLOR_GRAY + "\xe2\x88\x92 " + p1->m_title + COLOR_RESET);
                        } else if (p1->m_errors.size()) {
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
                            if (p1->m_level < p->m_run_level || p1->m_runtime_skip) {
                                // ctx.skip() called from the body counts as a
                                // skipped case and suppresses afterEach, exactly
                                // like an options-level skip (Node parity).
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

                    // node's spec reporter prints the diagnostics collected by
                    // ctx.diagnostic()/ctx.log() right under the test line.
                    bool printed = !p1->m_status
                        || mode > console_base::C_ERROR || p1->m_level >= p->m_run_level;

                    if (!p1->m_status)
                        outLog(console_base::C_INFO, logger::error() + str + COLOR_RESET);
                    else if (printed)
                        outLog(console_base::C_INFO, str);

                    if (printed)
                        for (i = 0; i < (int32_t)p1->m_diagnostics.size(); i++) {
                            exlib::string dstr(stack.size() * 2, ' ');
                            dstr.append("\xe2\x84\xb9 "); // U+2139 INFORMATION SOURCE
                            dstr.append(p1->m_diagnostics[i]);
                            outLog(console_base::C_INFO, dstr);
                        }

                    // afterEach runs AFTER the case is reported, as in Node: the
                    // case keeps its own result until a hook fails, which then
                    // rewrites it. Ancestors' hooks run after this level's, and
                    // hooks are called in declaration order (FIFO) — Node's
                    // createHook splices afterEach before ownAfterEachCount and
                    // runHook iterates forwards. A throwing hook stops the rest
                    // of the chain (Node's runHook throws out of the loop), and
                    // a failed before suppresses the whole afterEach phase: Node
                    // never starts the subtests in that case.
                    if (p1->m_level >= p->m_run_level && !before_failed && !p1->m_runtime_skip) {
                        v8::HandleScope handle_scope(isolate->m_isolate);

                        bool hook_failed = false;
                        for (j = (int32_t)stack.size() - 1; j >= 0 && !hook_failed; j--) {
                            p2 = stack[j];
                            for (i = 0; i < (int32_t)p2->m_hooks[HOOK_AFTERCASE].size(); i++) {
                                TryCatch try_catch;
                                v8::Local<v8::Function> func = p2->m_hooks[HOOK_AFTERCASE][i].Get(isolate->m_isolate);
                                v8::Local<v8::Value> ctx_arg = make_case_context(p1, CTX_TEST);
                                func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 1, &ctx_arg).IsEmpty();
                                if (try_catch.HasCaught()) {
                                    // afterEach failed: mark current test failed (even if it
                                    // passed), stop the remaining hooks (aligned with Node)
                                    exlib::string err = "[afterEach] " + GetException(try_catch, result_t(), false, true);
                                    bool was_passed = p1->m_errors.size() == 0;

                                    hook_failed = true;
                                    p1->m_errors.append(err);
                                    if (was_passed) {
                                        p1->m_status = false;
                                        p->m_status = false;
                                        p->m_fail++;
                                        p->m_pass--;

                                        // A hook failure that flips a passing case
                                        // is a real failure: it goes through the
                                        // same numbered channel, so it reaches the
                                        // failure list and the exit code (Node
                                        // fails the test and exits non-zero).
                                        snprintf(buf, sizeof(buf), "%d) ", ++errcnt);
                                        {
                                            exlib::string str1(buf);
                                            for (int32_t k = 1; k < (int32_t)stack.size(); k++) {
                                                str1.append(stack[k]->m_title);
                                                str1.append(" ", 1);
                                            }
                                            str1.append(p1->m_title);
                                            str1.append(" (afterEach hook)");
                                            names.append(COLOR_BOLD + str1 + COLOR_RESET);
                                        }
                                        if (mode <= console_base::C_ERROR)
                                            msgs.append(err);

                                        v8::Local<v8::Object> val = p->m_retVal_tests->Get(_context, p->m_pos - 1).ToLocalChecked().As<v8::Object>();
                                        val->Set(_context, isolate->NewString("status"), isolate->NewString("failed")).IsJust();
                                        val->Set(_context, isolate->NewString("trace"), isolate->NewString(err)).IsJust();

                                        outLog(console_base::C_ERROR, logger::error() + str + " [afterEach failed]" + COLOR_RESET);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }

                // Determine whether this is a describe: registered through
                // describe()/suite(), or (defensive) a node that turned out to
                // hold children and never had a body.
                if (p1->m_status && (is_suite || p1->m_subs.size()
                        || (!p1->m_describe_block.IsEmpty() && p1->m_block.IsEmpty()))) {
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
                // A suite whose ancestor's before hook failed never started in
                // Node (the parent throws before starting its subtests), so its
                // own after hooks do not run either. The suite that owns the
                // failing before still runs its after (Node calls after() from
                // the catch block).
                bool anc_before_failed = false;
                for (j = 1; j < (int32_t)stack.size() - 1; j++)
                    if (stack[j]->m_before_failed) {
                        anc_before_failed = true;
                        break;
                    }

                if (p->m_run_level != TEST_NONE && !anc_before_failed) {
                    v8::HandleScope handle_scope(isolate->m_isolate);

                    // Declaration order (FIFO), like Node's runHook; a throwing
                    // hook stops the remaining ones (Node throws out of the loop).
                    for (i = 0; i < (int32_t)p->m_hooks[HOOK_AFTER].size(); i++) {
                        TryCatch try_catch;
                        v8::Local<v8::Function> func = p->m_hooks[HOOK_AFTER][i].Get(isolate->m_isolate);
                        v8::Local<v8::Value> ctx_arg = make_case_context(p, CTX_SUITE);
                        func->Call(func->GetCreationContextChecked(), v8::Object::New(isolate->m_isolate), 1, &ctx_arg).IsEmpty();
                        if (try_catch.HasCaught()) {
                            // after failed：与 Node 一致 —— 子测试保持各自状态（已经打印过的 √
                            // 不再被改写），钩子失败本身作为一条独立失败记录：走叶子测试相同的
                            // 编号失败通道（计入退出码与失败清单），但不计入 tests/failed 计数
                            // （Node 的 fail 同样不含钩子失败，见 test/runner_hook_test.js）。
                            exlib::string err = "[after] " + GetException(try_catch, result_t(), false, true);

                            p->m_status = false;
                            p->m_errors.append(err);
                            p->m_retVal->Set(_context, isolate->NewString("trace"), isolate->NewString(err)).IsJust();

                            exlib::string errline(stack.size() * 2, ' ');
                            errline.append("[after failed] ");
                            errline.append(p->m_title);
                            outLog(console_base::C_ERROR, logger::error() + errline + COLOR_RESET);

                            if (mode > console_base::C_ERROR)
                                errorLog(err);
                            else if (mode == console_base::C_ERROR) {
                                exlib::string str1;

                                snprintf(buf, sizeof(buf), "%d) ", ++errcnt);
                                str1 = buf;
                                // stack.back() 就是本 suite（栈还未 pop），标题只取祖先链
                                for (j = 1; j < (int32_t)stack.size() - 1; j++) {
                                    str1.append(stack[j]->m_title);
                                    str1.append(" ", 1);
                                }
                                str1.append(p->m_title);
                                str1.append(" (after hook)");
                                names.append(COLOR_BOLD + str1 + COLOR_RESET);
                                msgs.append(err);
                            }
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
                if (p->m_cancel)
                    p->m_retVal->Set(_context, isolate->NewString("cancelled"), v8::Number::New(isolate->m_isolate, p->m_cancel)).IsJust();

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
                    p1->m_cancel += p->m_cancel;
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

        if (td->m_root->m_cancel) {
            snprintf(buf, sizeof(buf), (COLOR_GRAY + "  \xe2\x88\x92 %d tests cancelled" + COLOR_RESET).c_str(), td->m_root->m_cancel);
            outLog(console_base::C_INFO, buf);
        }

        outLog(console_base::C_INFO, "");

        for (i = 0; i < (int32_t)msgs.size(); i++) {
            outLog(console_base::C_INFO, names[i]);
            outLog(console_base::C_INFO, logger::error() + msgs[i] + COLOR_RESET);
        }

        retVal = td->m_root->m_retVal;

        clear();

        // Everything after this point is event-loop drain: an exception there
        // has no test left to fail, so test_report_late_error() takes over.
        s_test_run_finished = true;

        if (errcnt)
            process_base::set_exitCode(1);

        return result_t();
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

    // Ancestor titles joined by single spaces (excludes the root), i.e. Node's
    // getTestNameWithAncestors(). Kept for the --test-name-pattern full-name
    // match; equal to m_title for top-level nodes.
    exlib::string m_full_name;

    // Same chain in Node's display form ('suite > sub > case') for
    // context.fullName. The root is excluded, as in Node.
    exlib::string m_display_name;

    int32_t m_total = 0;
    int32_t m_pass = 0;
    int32_t m_fail = 0;
    int32_t m_skip = 0;
    int32_t m_todo = 0;
    int32_t m_cancel = 0;

    bool m_status = true;

    // --test-name-pattern chain rule: a title on the root chain matched.
    // Computed when the node is picked during the run and reused by the
    // suite_visible gate and by the node's own children.
    bool m_chain_matched = false;

    // --test-skip-pattern chain rule: a title on the root chain matched.
    bool m_chain_skipped = false;

    // todo inherited from an enclosing describe.todo/suite.todo (Node parity).
    bool m_todo_inherited = false;

    // context.skip() / context.todo() called from inside the body (Node parity):
    // a runtime skip counts as skipped and suppresses this case's afterEach,
    // a runtime todo classifies the outcome as todo.
    bool m_runtime_skip = false;
    bool m_runtime_todo = false;

    // Registered through describe()/suite() rather than it(). Suites stay
    // identifiable after expand_describe() has cleared m_describe_block.
    bool m_is_suite = false;

    // ctx.diagnostic() / ctx.log() text, printed under the case result.
    QuickArray<exlib::string> m_diagnostics;

    // Cancelled by an ancestor's failing before hook (Node counts those as
    // cancelled instead of failed).
    bool m_cancelled = false;

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

    // The stored block is this wrapper behind util.sync, so the first argument
    // is the context the runner passed; the appended callback arrives last.
    int32_t len = args.Length();
    std::vector<v8::Local<v8::Value>> argv(len);
    argv[0] = len > 0 ? args[0] : v8::Object::New(isolate->m_isolate);
    for (int32_t i = 1; i < len; i++)
        argv[i] = args[i];

    v8::Local<v8::Value> result;
    if (func->Call(context, args.This(), (int32_t)argv.size(), argv.data()).ToLocal(&result))
        args.GetReturnValue().Set(result);
}

// The object handed to a test body (Node's TestContext) and to a suite body or
// hook (SuiteContext). It carries the members a case can act on here:
// name/fullName/filePath/passed, diagnostic(), log(), skip(), todo() and the
// shared assert module. Node's test()/before()/plan()/mock()/waitFor() depend on
// subtests and a per-case plan, neither of which fibjs has.
static _case* ctx_case(v8::Local<v8::Value> data)
{
    return (_case*)data.As<v8::External>()->Value();
}

static void ctx_diagnostic(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);

    if (args.Length() < 1)
        return;

    // ctx.log() shares the body: node routes both through the reporter, and the
    // spec reporter prints them the same way.
    ctx_case(args.Data())->add_diagnostic(isolate->toString(args[0]));
}

static void ctx_skip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ctx_case(args.Data())->request_skip();
}

static void ctx_todo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ctx_case(args.Data())->request_todo();
}

static void ctx_name_getter(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    Isolate* isolate = Isolate::current(info);
    info.GetReturnValue().Set(isolate->NewString(ctx_case(info.Data())->context_name()));
}

static void ctx_full_name_getter(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    Isolate* isolate = Isolate::current(info);
    info.GetReturnValue().Set(isolate->NewString(ctx_case(info.Data())->context_full_name()));
}

static void ctx_passed_getter(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    info.GetReturnValue().Set(v8::Boolean::New(Isolate::current(info)->m_isolate,
        ctx_case(info.Data())->context_passed()));
}

static void ctx_file_getter(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    Isolate* isolate = Isolate::current(info);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> v = context->Global()
                                 ->Get(context, isolate->NewString("__fibjs_test_entry_file"))
                                 .FromMaybe(v8::Local<v8::Value>());
    if (v.IsEmpty() || !v->IsString())
        v = context->Global()
                ->Get(context, isolate->NewString("__filename"))
                .FromMaybe(v8::Local<v8::Value>());
    if (!v.IsEmpty())
        info.GetReturnValue().Set(v);
}

// Build the context object for a case (or a suite). The assert object is the
// shared assert module, so `ctx.assert.equal(...)` works like `test.assert`.
static v8::Local<v8::Object> make_case_context(_case* c, int32_t kind)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> ctx = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Value> ext = v8::External::New(isolate->m_isolate, c);

    ctx->SetNativeDataProperty(context, isolate->NewString("name"), ctx_name_getter, NULL, ext).IsJust();
    ctx->SetNativeDataProperty(context, isolate->NewString("fullName"), ctx_full_name_getter, NULL, ext).IsJust();
    ctx->SetNativeDataProperty(context, isolate->NewString("filePath"), ctx_file_getter, NULL, ext).IsJust();
    ctx->SetNativeDataProperty(context, isolate->NewString("passed"), ctx_passed_getter, NULL, ext).IsJust();

    ctx->Set(context, isolate->NewString("diagnostic"),
                 isolate->NewFunction("diagnostic", ctx_diagnostic, ext)).IsJust();
    ctx->Set(context, isolate->NewString("log"),
                 isolate->NewFunction("log", ctx_diagnostic, ext)).IsJust();

    if (kind == CTX_TEST) {
        ctx->Set(context, isolate->NewString("skip"),
                     isolate->NewFunction("skip", ctx_skip, ext)).IsJust();
        ctx->Set(context, isolate->NewString("todo"),
                     isolate->NewFunction("todo", ctx_todo, ext)).IsJust();
    }

    ctx->Set(context, isolate->NewString("assert"),
                 assert_base::class_info().getModule(isolate)).IsJust();

    return ctx;
}

// Called from Fiber.cpp whenever an exception escapes a JS scope unnoticed.
// Errors during the run are observed by the runner (assert.throws, hook
// failures, ...), so they are ignored here; only post-run errors fail the run.
// ReportException has already printed the error and its stack, so this only
// adds the notice that explains why the run fails.
void test_report_late_error()
{
    if (!s_test_run_finished)
        return;

    // A worker isolate's post-run error belongs to the worker (it already reached
    // the 'error' event / its own exit code); only the main isolate's errors are
    // "activity after the tests ended".
    Isolate* isolate = Isolate::current();
    if (isolate->m_parent_worker)
        return;

    process_base::set_exitCode(1);
    outLog(console_base::C_ERROR, logger::error()
                + "Error: asynchronous activity after the tests ended; the error above fails the run." + COLOR_RESET);
}

// Node's arity rule (lib/internal/test_runner/test.js: `if (this.fn.length ===
// runArgs.length - 1)` with runArgs = [fn, ctx, ctx]): a body declared with two
// or more parameters gets (ctx, done), anything else gets (ctx). One-argument
// bodies used to receive `done`; they now receive the context, like Node.
// The diagnostic runs first, on the user's own function: wrapping an async body
// yields a native wrapper whose source text no longer names its parameters.
inline v8::Local<v8::Function> wrapFunction(v8::Local<v8::Function> func, const exlib::string& name)
{
    warn_legacy_callback(func, name);

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
        }
    }

    return func;
}

// Parameter names that used to mean "this body takes a done callback" before
// the node arity rule landed. A one-parameter body using one of them is almost
// always left-over callback-style code, so the runner warns once. The name is
// only used for the warning: dispatch still follows node's arity rule.
static bool looks_like_callback_name(const exlib::string& name)
{
    static const char* names[] = { "done", "cb", "callback", "next", "resolve", "finish", "complete" };

    for (int32_t i = 0; i < (int32_t)(sizeof(names) / sizeof(names[0])); i++)
        if (name == names[i])
            return true;

    return false;
}

// Read the first parameter name out of a function's source text. Returns the
// empty string when the source is unavailable (native/bound functions and
// precompiled jsc scripts stringify to opaque text), which simply means "no
// warning", never "callback style".
static exlib::string first_parameter_name(v8::Local<v8::Function> func)
{
    Isolate* isolate = Isolate::current();

    v8::Local<v8::Value> src = func->ToString(isolate->context()).FromMaybe(v8::Local<v8::Value>());
    if (src.IsEmpty() || !src->IsString())
        return exlib::string();

    exlib::string text = isolate->toString(src);
    size_t i = 0;
    size_t len = text.length();

    auto skip_space = [&]() {
        while (i < len && (text[i] == ' ' || text[i] == '\t' || text[i] == '\n' || text[i] == '\r'))
            i++;
    };

    auto is_ident = [](char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
            || c == '_' || c == '$';
    };

    // Skip the `async` prefix; a `function` keyword hides the parameters behind
    // an optional name, so walk to the opening parenthesis in that case.
    skip_space();
    if (text.length() >= i + 5 && text.substr(i, 5) == "async") {
        i += 5;
        skip_space();
    }
    if (text.length() >= i + 8 && text.substr(i, 8) == "function")
        while (i < len && text[i] != '(')
            i++;

    // `(done) =>`, `function (done)` and the unparenthesised `done =>` all end
    // up reading the same identifier.
    if (i < len && text[i] == '(') {
        i++;
        skip_space();
    }

    size_t start = i;
    while (i < len && is_ident(text[i]))
        i++;

    return i > start ? text.substr(start, i - start) : exlib::string();
}

// One warning per run, not per case: the same file usually has many of these.
static bool s_legacy_callback_warned = false;

static void warn_legacy_callback(v8::Local<v8::Function> func, const exlib::string& name)
{
    if (s_legacy_callback_warned || func.IsEmpty())
        return;

    Isolate* isolate = Isolate::current();
    int32_t length = isolate->toInt32Value(JSValue(func->Get(isolate->context(), isolate->NewString("length"))));
    if (length != 1)
        return;

    exlib::string param = first_parameter_name(func);
    if (param.empty() || !looks_like_callback_name(param))
        return;

    s_legacy_callback_warned = true;

    // No trailing newline: the logger adds one (and the reset after it).
    outLog(console_base::C_WARN, "warning: test '" + name + "' looks like the retired callback style (parameter named '"
            + param + "'), but a single parameter now receives the test context. Write (ctx, done) => {} if the case needs a done callback.");
}

static int32_t get_level_from_options(v8::Local<v8::Object> options, int32_t default_level)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> v;

    // Node treats the marks as "provided and not false" (`skip !== undefined &&
    // skip !== false`), so any other value — `''` included — counts as set.
    if (options->Get(context, isolate->NewString("skip")).ToLocal(&v) && !v->IsUndefined() && !v->IsFalse())
        return _case::TEST_SKIP;

    if (options->Get(context, isolate->NewString("todo")).ToLocal(&v) && !v->IsUndefined() && !v->IsFalse())
        return _case::TEST_TODO;

    if (options->Get(context, isolate->NewString("only")).ToLocal(&v) && !v->IsUndefined() && !v->IsFalse())
        return _case::TEST_ONLY;

    return default_level;
}

result_t test_base::_function(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block, name), _case::TEST_NORMAL);
}

result_t test_base::_function(exlib::string name, v8::Local<v8::Object> options, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block, name), get_level_from_options(options, _case::TEST_NORMAL));
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
    return _case::it(name, wrapFunction(block, name), _case::TEST_SKIP);
}

result_t test_base::xit(exlib::string name, v8::Local<v8::Function> block)
{
    return skip(name, block);
}

result_t test_base::only(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block, name), _case::TEST_ONLY);
}

result_t test_base::oit(exlib::string name, v8::Local<v8::Function> block)
{
    return only(name, block);
}

result_t test_base::todo(exlib::string name, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block, name), _case::TEST_TODO);
}

result_t test_base::todo(exlib::string name, v8::Local<v8::Object> options, v8::Local<v8::Function> block)
{
    return _case::it(name, wrapFunction(block, name), get_level_from_options(options, _case::TEST_TODO));
}

result_t test_base::todo(exlib::string name)
{
    return _case::it(name, v8::Local<v8::Function>(), _case::TEST_TODO);
}

result_t test_base::before(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_BEFORE, wrapFunction(func, "hook"));
}

result_t test_base::after(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_AFTER, wrapFunction(func, "hook"));
}

result_t test_base::beforeEach(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_BEFORECASE, wrapFunction(func, "hook"));
}

result_t test_base::afterEach(v8::Local<v8::Function> func)
{
    return _case::set_hook(HOOK_AFTERCASE, wrapFunction(func, "hook"));
}

static int32_t s_watchdog_ms = 10000;

extern exlib::atomic g_ExtStringCount;

// Tree glyphs used to paint the class hierarchy: "├─ ", "└─ ", "│  ". Written
// as UTF-8 escapes to keep the sources ASCII-only (same convention as
// util_format_table.cpp); the runtime strings are regular UTF-8.
static const char s_tree_branch[] = "\xe2\x94\x9c\xe2\x94\x80 ";
static const char s_tree_last[] = "\xe2\x94\x94\xe2\x94\x80 ";
static const char s_tree_bar[] = "\xe2\x94\x82  ";

// Recursively paint the class tree returned by ClassInfo::dump():
// {class, objects, inherits:[child, ...]}
//
// 'objects' counts every live instance of the class *and* of any derived class
// (each constructor in the inheritance chain bumps its own class counter), so a
// child count is a subset of its parent count. Painting the tree makes that
// relation readable, and 'direct' (= own count - sum of the child counts) is the
// number of instances of the exact class: a class that shows 'direct' is where
// instances actually sit, classes without it are pure roll-ups of their children.
static void collect_native_objects(v8::Local<v8::Value> v, v8::Local<v8::Context> ctx, Isolate* isolate,
    exlib::string& report, const exlib::string& indent, const char* branch, bool is_last)
{
    if (!v->IsObject())
        return;

    v8::Local<v8::Object> o = v.As<v8::Object>();
    v8::Local<v8::Value> cls = o->Get(ctx, isolate->NewString("class")).FromMaybe(v8::Local<v8::Value>());
    v8::Local<v8::Value> count = o->Get(ctx, isolate->NewString("objects")).FromMaybe(v8::Local<v8::Value>());
    v8::Local<v8::Value> inherits = o->Get(ctx, isolate->NewString("inherits")).FromMaybe(v8::Local<v8::Value>());

    if (cls.IsEmpty() || !cls->IsString() || count.IsEmpty() || !count->IsInt32())
        return;

    v8::Local<v8::Array> arr;
    uint32_t children = 0;

    if (!inherits.IsEmpty() && inherits->IsArray()) {
        arr = inherits.As<v8::Array>();
        children = arr->Length();
    }

    char buf[64];
    int32_t cnt = count->Int32Value(ctx).FromMaybe(0);
    int32_t direct = cnt;

    for (uint32_t i = 0; i < children; i++) {
        v8::Local<v8::Value> child = arr->Get(ctx, i).FromMaybe(v8::Local<v8::Value>());
        v8::Local<v8::Value> child_count;

        if (!child.IsEmpty() && child->IsObject())
            child_count = child.As<v8::Object>()->Get(ctx, isolate->NewString("objects")).FromMaybe(v8::Local<v8::Value>());

        if (!child_count.IsEmpty() && child_count->IsInt32())
            direct -= child_count->Int32Value(ctx).FromMaybe(0);
    }

    report.append(indent);
    report.append(branch);
    report.append(isolate->toString(cls));
    report.append(": ");
    snprintf(buf, sizeof(buf), "%d", cnt);
    report.append(buf);

    if (children && direct > 0) {
        snprintf(buf, sizeof(buf), " (direct: %d)", direct);
        report.append(buf);
    }
    report.append(1, '\n');

    exlib::string child_indent(indent);

    // Children of the root line up under its name, deeper levels extend the
    // vertical bar of a parent that is not the last of its siblings
    if (*branch)
        child_indent.append(is_last ? "   " : s_tree_bar);

    for (uint32_t i = 0; i < children; i++)
        collect_native_objects(arr->Get(ctx, i).FromMaybe(v8::Local<v8::Value>()), ctx, isolate,
            report, child_indent, i + 1 == children ? s_tree_last : s_tree_branch, i + 1 == children);
}

// Hang diagnostic report: fibers + stack, native object counts after GC, memory info
//
// The report is UTF-8 (tree glyphs, script paths, class names) and is printed
// through errorLog(), i.e. the console.error() path: it goes out through the
// isolate's stderr stream, where libuv converts UTF-8 to UTF-16 for a Windows
// console (uv_tty -> WriteConsoleW). Handing the raw bytes to the C runtime's
// stderr instead lets a Windows console decode them with the active OEM code
// page, which turns the tree glyphs into mojibake. Redirected output (files,
// pipes) gets the same bytes either way.
static void report_watchdog(Isolate* isolate)
{
    exlib::string report;
    char buf[256];

    // The tag is printed once, as the header of the whole block: the lines below
    // are all watchdog output, so they do not need a per-line prefix
    report.append("[test-watchdog] process alive ");
    snprintf(buf, sizeof(buf), "%d", s_watchdog_ms);
    report.append(buf);
    report.append("ms after test run ended\n");

    // 1) Force GC: also drains the isolate's m_weak pending-delete list
    //    (fb_GCCallback), so nativeObjects reflects objects that are truly alive
    isolate->m_isolate->LowMemoryNotification();

    // 2) Alive fibers and their stacks: the count first, then one block per
    //    fiber (the watchdog's own diagnostic fiber is excluded)
    int64_t self_id = -1;
    JSFiber* self = JSFiber::current();
    if (self)
        self->get_id(self_id);

    int32_t fiber_count = 0;
    exlib::linkitem* p = isolate->m_fibers.head();

    while (p) {
        JSFiber* fb = (JSFiber*)p;
        int64_t id;

        fb->get_id(id);
        if (id != self_id)
            fiber_count++;

        p = p->m_next;
    }

    snprintf(buf, sizeof(buf), "fibers: %d\n", fiber_count);
    report.append(buf);

    p = isolate->m_fibers.head();

    while (p) {
        JSFiber* fb = (JSFiber*)p;
        int64_t id;

        fb->get_id(id);

        if (id != self_id) {
            int32_t usage;
            exlib::string stack;

            fb->get_stack_usage(usage);
            fb->get_stack(stack);

            snprintf(buf, sizeof(buf), "  fiber #%lld (stack_usage=%d):\n", (long long)id, usage);
            report.append(buf);

            // Frames come pre-indented ("    at ...") from traceInfo, keep them
            // verbatim so a pasted stack stays readable
            if (!stack.empty()) {
                report.append(stack);
                report.append(1, '\n');
            }
        }

        p = p->m_next;
    }

    // 3) Native object counts per class (after GC), painted as the class tree:
    //    a class count includes its derived classes, 'direct' marks the classes
    //    that hold instances of their exact type (see collect_native_objects)
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> objs;

    report.append("nativeObjects (after gc):\n");
    object_base::class_info().dump(objs);

    if (objs.IsEmpty())
        report.append("  (none)\n");
    else {
        report.append("  # count includes derived classes, direct = instances of the exact class\n");
        collect_native_objects(objs, _context, isolate, report, "  ", "", true);
    }

    // 4) Memory info
    size_t rss = 0;
    uv_resident_set_memory(&rss);
    v8::HeapStatistics hs;
    isolate->m_isolate->GetHeapStatistics(&hs);

    report.append("memory: rss=");
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
    errorLog(report);
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
        m_isolate->sync([isolate = m_isolate]() {
            v8::HandleScope handle_scope(isolate->m_isolate);
            JSFiber::EnterJsScope s;

            // Timer fired while the process is still alive => abnormal hang:
            // collect diagnostics and force exit (124)
            report_watchdog(isolate);

            // Clean up leaked child processes before the force exit: spawned
            // children are separate OS processes and would survive _exit(124),
            // then keep running after the test run (holding ports, etc.) and
            // interfere with later test runs.
            int32_t killed = child_process_kill_alive_children();

            if (killed > 0) {
                char buf[128];
                exlib::string msg;

                snprintf(buf, sizeof(buf),
                    "[test-watchdog] killed %d leaked child process(es)\n", killed);
                msg.append(buf);

                // Same output path as the report above (see report_watchdog)
                errorLog(msg);
            }

            process_base::exit(124);

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

    isolate->sync([isolate, mode, td]() {
        v8::HandleScope handle_scope(isolate->m_isolate);
        JSFiber::EnterJsScope s;

        // --test-name-pattern / --test-skip-pattern: opt_tools/test.js stores
        // the compiled RegExp[] on the global (non-enumerable) before loading
        // test files.
        v8::Local<v8::Context> _context = isolate->context();
        {
            v8::Local<v8::Object> g = _context->Global();
            const char* keys[2] = { "__fibjs_test_name_patterns", "__fibjs_test_skip_patterns" };
            QuickArray<v8::Global<v8::RegExp>>* targets[2] = { &td->m_name_patterns, &td->m_skip_patterns };

            for (int32_t k = 0; k < 2; k++) {
                v8::Local<v8::Value> v = g->Get(_context, isolate->NewString(keys[k]))
                                            .FromMaybe(v8::Local<v8::Value>());
                if (!v.IsEmpty() && v->IsArray()) {
                    v8::Local<v8::Array> arr = v.As<v8::Array>();
                    for (uint32_t i = 0; i < arr->Length(); i++) {
                        v8::Local<v8::Value> e = arr->Get(_context, i).FromMaybe(v8::Local<v8::Value>());
                        if (!e.IsEmpty() && e->IsRegExp()) {
                            int32_t n = (int32_t)targets[k]->size();
                            targets[k]->resize(n + 1);
                            (*targets[k])[n].Reset(isolate->m_isolate, e.As<v8::RegExp>());
                        }
                    }
                }
            }
        }

        v8::Local<v8::Object> ret;
        result_t hr = _case::run(mode, ret);
        if (hr.failed()) {
            ThrowResult(hr);
        }

        // Arm the watchdog after the run: if the process does not exit within
        // s_watchdog_ms, treat it as an abnormal hang (resources not released),
        // collect diagnostics and force exit
        arm_test_watchdog(isolate, s_watchdog_ms);

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
        td->m_running->m_errors.append(GetException(try_catch, result_t(), false, true));
        try_catch.ReThrow();
    }

    ev->set();
}

result_t test_base::mustCall(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    TestData* td = TestData::current();
    if (!td->m_running)
        return result_t::invalid_call();

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);
    obj_ptr<Event_base> ev = new Event();

    td->m_running->m_evs.append(ev);

    _data->Set(_context, isolate->NewString("func"), func).IsJust();
    _data->Set(_context, isolate->NewString("ev"), ev->wrap()).IsJust();
    _data->Set(_context, isolate->NewString("case"), v8::External::New(isolate->m_isolate, td->m_running)).IsJust();

    retVal = isolate->NewFunction("mustNotCall", must_call, _data);

    return result_t();
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

    v8::Local<v8::Value> exp = FillError(result_t(CALL_E_EXCEPTION), "This function must never be called.");
    td->m_running->m_errors.append(GetException(exp, false, true));
    ThrowError(exp);
}

result_t test_base::mustNotCall(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal)
{
    TestData* td = TestData::current();
    if (!td->m_running)
        return result_t::invalid_call();

    Isolate* isolate = Isolate::current();
    retVal = isolate->NewFunction("mustNotCall", not_call, v8::External::New(isolate->m_isolate, td->m_running));
    return result_t();
}

result_t test_base::mustNotCall(v8::Local<v8::Function>& retVal)
{
    TestData* td = TestData::current();
    if (!td->m_running)
        return result_t::invalid_call();

    Isolate* isolate = Isolate::current();
    retVal = isolate->NewFunction("mustNotCall", not_call, v8::External::New(isolate->m_isolate, td->m_running));
    return result_t();
}

result_t test_base::get_slow(int32_t& retVal)
{
    retVal = s_slow;
    return result_t();
}

result_t test_base::set_slow(int32_t newVal)
{
    s_slow = newVal;
    return result_t();
}
}
