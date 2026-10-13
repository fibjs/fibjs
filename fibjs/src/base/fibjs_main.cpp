/*
 * fibjs_main.cpp
 *
 *  Created on: May 28, 2018
 *      Author: lion
 */

#include "fibjs.h"
#include "ifs/process.h"
#include "ifs/console.h"
#include "options.h"
#include "SandBox.h"
#include "Fiber.h"

namespace fibjs {

void run_test(int32_t mode);

static void main_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);

    result_t hr = isolate->m_topSandbox->run_main(isolate->m_fname);
    if (hr.ok()) {
        run_test(console_base::C_ERROR);
    }

    if (hr.failed()) {
        THROW_ERROR();
    }
}

result_t FiberProcJsEntry(Isolate* isolate)
{
    JSFiber::EnterJsScope s;

    result_t hr;

    isolate->start_profiler();
    v8::Local<v8::Context> _context = isolate->context();

    isolate->m_topSandbox = new SandBox();
    isolate->m_topSandbox->addBuiltinModules();
    isolate->m_topSandbox->installBuffer();

    if (!isolate->m_fname.empty()) {
        v8::Local<v8::Value> result;
        v8::Local<v8::Function> _main_func = isolate->NewFunction("main", main_stub);
        if (_main_func.IsEmpty() || !_main_func->Call(_context, _main_func, 0, NULL).ToLocal(&result))
            hr = result_t(CALL_E_JAVASCRIPT);
    } else {
        RootModule* pModule = RootModule::g_root;
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Object> glob = _context->Global();

        while (pModule) {
            glob->DefineOwnProperty(_context,
                    isolate->NewString(pModule->name()),
                    pModule->getModule(isolate),
                    (v8::PropertyAttribute)(v8::DontEnum))
                .IsJust();
            pModule = pModule->m_next;
        }

        if (g_exec_print && !isolate->m_jsCode.empty())
            hr = isolate->m_topSandbox->eval_print(isolate->m_jsCode);
        else
            hr = isolate->m_topSandbox->repl(isolate->m_jsCode);
    }

    // The entry result belongs to the isolate, not to the sync() hand-off: the
    // caller (Runtime::FirstFiber) is gone when the event loop runs dry, and
    // this value is what decides the process exit code. Storing it here is the
    // explicit hand-off that used to ride SyncFunc::js_invoke's return value.
    isolate->m_hr = hr;

    return hr;
}
}