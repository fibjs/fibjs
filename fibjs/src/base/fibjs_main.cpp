/*
 * fibjs_main.cpp
 *
 *  Created on: May 28, 2018
 *      Author: lion
 */

#include "fibjs.h"
#include "ifs/global.h"
#include "SandBox.h"
#include "Fiber.h"

namespace fibjs {

static void main_stub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    V8_SCOPE(isolate->m_isolate);
    v8::Local<v8::Array> argv;

    global_base::get_argv(argv);
    result_t hr = isolate->m_topSandbox->run_main(isolate->m_fname, argv);

    if (hr < 0) {
        THROW_ERROR();
    }
}

result_t FiberProcJsEntry(Isolate* isolate)
{
    JSFiber::EnterJsScope s;

    isolate->start_profiler();
    v8::Local<v8::Context> _context = isolate->context();

    if (!isolate->m_fname.empty()) {
        v8::Local<v8::Value> result;
        v8::Local<v8::Function> _main_func = isolate->NewFunction("main", main_stub);
        result = _main_func->Call(_context, _main_func, 0, NULL).FromMaybe(v8::Local<v8::Value>());
        if (result.IsEmpty())
            s.m_hr = CALL_E_JAVASCRIPT;
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

        s.m_hr = isolate->m_topSandbox->repl(isolate->m_jsCode);
    }

    return s.m_hr;
}
}