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

// Whether a failed CJS compilation was really an ES module is decided in
// esm_detect.cpp: `.js` and `.ts` are ambiguous in the same way and share it.

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

            if (shouldRetryAsESM(err_msg, src, name, false)) {
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
