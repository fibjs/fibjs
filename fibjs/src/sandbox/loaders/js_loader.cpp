/*
 * js_loader.cpp
 *
 *  Created on: Aug 19, 2024
 *      Author: lion
 */

#include "object.h"
#include "Buffer.h"
#include "loaders.h"

namespace fibjs {

// Error messages that unambiguously indicate ESM syntax (import/export/import.meta).
// When CJS compilation fails with one of these, the file should be retried as ESM.
static const char* esm_syntax_error_messages[] = {
    "Cannot use import statement outside a module",
    "Unexpected token 'export'",
    "Cannot use 'import.meta' outside a module"
};

// Error messages that may be valid in ESM but not in CJS.
// For these, we try to compile the source as an ES module to verify.
static const char* throws_only_in_cjs_error_messages[] = {
    "await is only valid in async functions and the top level bodies of modules",
    "Identifier 'module' has already been declared",
    "Identifier 'exports' has already been declared",
    "Identifier 'require' has already been declared",
    "Identifier '__filename' has already been declared",
    "Identifier '__dirname' has already been declared"
};

// Check if the error message from CJS compilation indicates that the source
// uses ESM syntax and should be retried as an ES module.
// This implements the require(esm) feature similar to Node.js v22.12+.
static bool shouldRetryAsESM(exlib::string err_msg, Buffer_base* src, exlib::string name)
{
    // Primary check: error messages that unambiguously indicate ESM syntax
    for (auto& pattern : esm_syntax_error_messages) {
        if (err_msg.find(pattern) != exlib::string::npos)
            return true;
    }

    // Secondary check: error messages that are valid in ESM but not CJS.
    // Try to compile the source as an ES module to verify.
    bool maybe_valid_in_esm = false;
    for (auto& pattern : throws_only_in_cjs_error_messages) {
        if (err_msg.find(pattern) != exlib::string::npos) {
            maybe_valid_in_esm = true;
            break;
        }
    }

    if (!maybe_valid_in_esm)
        return false;

    // Try to compile as ESM to verify the source is valid ES module syntax
    Isolate* isolate = Isolate::current();
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

result_t js_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs)
{
    result_t hr;
    SandBox::ModuleType type;

    hr = ctx->m_sb->resolveModuleType(name, type);
    if (hr)
        return hr;

    if (type == SandBox::kCommonJS) {
        hr = m_cjs.run(ctx, src, name, arg_names, args, in_cjs);
        if (hr >= 0)
            return hr;

        // If CJS compilation failed, check if the source uses ESM syntax
        // and retry as ESM (supporting require(esm) like Node.js v22.12+).
        // CALL_E_EXCEPTION indicates a stored Runtime error (syntax error),
        // as opposed to CALL_E_JAVASCRIPT (pending V8 exception at runtime).
        if (hr == CALL_E_EXCEPTION) {
            exlib::string err_msg = Runtime::errMessage();

            if (shouldRetryAsESM(err_msg, src, name)) {
                if (in_cjs)
                    return m_mjs.run(ctx, src, name, arg_names, args, in_cjs);

                // in_cjs is false: the file uses ESM syntax but was loaded via
                // the CJS path (e.g., a .js file with import/export but no
                // "type": "module" in package.json, imported from an ESM module).
                // Set a sentinel error so the ESM importer (esm_importer::resove_module)
                // can detect it and retry the module as ESM.
                return CHECK_ERROR(Runtime::setError(
                    "SandBox: ESM syntax detected; retry as ES module."));
            }

            // Not ESM syntax, restore the original error
            return CHECK_ERROR(Runtime::setError(err_msg));
        }

        return hr;
    } else if (type == SandBox::kESModule)
        return m_mjs.run(ctx, src, name, arg_names, args, in_cjs);

    return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));
}
}
