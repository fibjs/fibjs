/*
 * esm_detect.cpp
 *
 *  Created on: Sep 27, 2026
 *      Author: lion
 */

#include "object.h"
#include "Buffer.h"
#include "loaders.h"

namespace fibjs {

// A file is "ambiguous" when nothing pins its module system down: the extension
// is the plain one (.js or .ts) and the nearest package.json has no "type"
// field. Node.js runs such a file as CommonJS first and, when the parser
// rejects it because it found ES module syntax, runs it again as an ES module.
// `.js` and `.ts` are ambiguous in exactly the same way, so the decision is
// shared instead of living inside one of the two loaders.
//
// Note the asymmetry with `.cts`/`.mts`: those extensions do pin the module
// system down, so a `.cts` file that says `export` is simply broken and must
// not be rescued.

// Error messages that unambiguously indicate ESM syntax (import/export/import.meta).
static const char* esm_syntax_error_messages[] = {
    "Cannot use import statement outside a module",
    "Unexpected token 'export'",
    "Cannot use 'import.meta' outside a module"
};

// Error messages that may be valid in ESM but not in CJS. For these the message
// alone cannot decide, so the source is compiled as a module to find out.
static const char* throws_only_in_cjs_error_messages[] = {
    "await is only valid in async functions and the top level bodies of modules",
    "Identifier 'module' has already been declared",
    "Identifier 'exports' has already been declared",
    "Identifier 'require' has already been declared",
    "Identifier '__filename' has already been declared",
    "Identifier '__dirname' has already been declared"
};

bool shouldRetryAsESM(exlib::string err_msg, Buffer_base* src, exlib::string name, bool typescript)
{
    // Primary check: error messages that unambiguously indicate ESM syntax.
    for (auto& pattern : esm_syntax_error_messages) {
        if (err_msg.find(pattern) != exlib::string::npos)
            return true;
    }

    // Secondary check: messages that are valid in ESM but not in CJS.
    bool maybe_valid_in_esm = false;
    for (auto& pattern : throws_only_in_cjs_error_messages) {
        if (err_msg.find(pattern) != exlib::string::npos) {
            maybe_valid_in_esm = true;
            break;
        }
    }

    if (!maybe_valid_in_esm)
        return false;

    Isolate* isolate = Isolate::current();

    // TypeScript has to be stripped before the probe. The raw source is not
    // JavaScript, so V8 would reject it as a module for reasons that have
    // nothing to do with the module system and the probe would answer "not a
    // module" every time. ts_compile() caches, and this is the failure path.
    obj_ptr<Buffer_base> js_src;
    if (typescript) {
        if (cts_Loader::ts_compile(isolate, src, js_src) < 0)
            return false;
        src = js_src;
    }

    // Try to compile as ESM to verify the source is valid ES module syntax
    v8::HandleScope handle_scope(isolate->m_isolate);

    Buffer* buf = Buffer::Cast(src);
    const char* data = (const char*)buf->data();
    size_t length = buf->length();

    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
    pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, 0));
    v8::ScriptOrigin so_origin(isolate->NewString(name), 0, 0, false,
        -1, v8::Local<v8::Value>(), false, false, true, pargs);

    v8::ScriptCompiler::Source source(isolate->NewString(data, length), so_origin);

    TryCatch try_catch;
    v8::Local<v8::Module> module = v8::ScriptCompiler::CompileModule(isolate->m_isolate, &source)
                                       .FromMaybe(v8::Local<v8::Module>());

    return !module.IsEmpty();
}

}
