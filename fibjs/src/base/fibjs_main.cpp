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
result_t main_fiber(Isolate* isolate)
{
    JSFiber::scope s;

    isolate->start_profiler();

    if (!isolate->m_fname.empty()) {
        v8::Local<v8::Array> argv;

        global_base::get_argv(argv);
        s.m_hr = isolate->m_topSandbox->run_main(isolate->m_fname, argv);
    } else {
        v8::Local<v8::Array> cmds = v8::Array::New(isolate->m_isolate);
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

        s.m_hr = isolate->m_topSandbox->repl(cmds);
    }

    return s.m_hr;
}
}