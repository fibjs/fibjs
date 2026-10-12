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
    SandBox::ModuleTypeInfo info;

    hr = ctx->m_sb->resolveModuleType(name, info);
    if (hr.failed())
        return hr;

    if (info.type == SandBox::kCommonJS) {
        hr = m_cjs.run(ctx, src, name, arg_names, args, in_cjs);
        if (hr.ok())
            return hr;

        // If CJS compilation failed, check if the source uses ESM syntax
        // and retry as ESM (supporting require(esm) like Node.js v22.12+).
        // CALL_E_EXCEPTION indicates a stored description (syntax error), as
        // opposed to CALL_E_JAVASCRIPT (pending V8 exception at runtime). The
        // description rides with the value.
        if (hr.is_exception()) {
            const ErrorPayload* p = hr.payload();
            exlib::string err_msg = p ? p->message : exlib::string();

            // Detection is only for files nothing else has classified. When
            // package.json says "type": "commonjs", `export` in the file is an
            // error rather than a module to discover - Node.js draws the same
            // line and refuses to reparse such a file.
            if (!info.explicitType && shouldRetryAsESM(err_msg, src, name, false)) {
                if (in_cjs)
                    return m_mjs.run(ctx, src, name, arg_names, args, in_cjs);

                // in_cjs is false: the file uses ESM syntax but was loaded via
                // the CJS path (e.g., a .js file with import/export but no
                // "type": "module" in package.json, imported from an ESM module).
                // Set a sentinel error so the ESM importer (esm_importer::resove_module)
                // can detect it and retry the module as ESM.
                return result_t::make(CALL_E_EXCEPTION).with_message("SandBox: ESM syntax detected; retry as ES module.");
            }

            // Not ESM syntax, restore the original error
            return result_t::make(CALL_E_EXCEPTION).with_message(err_msg);
        }

        return hr;
    } else if (info.type == SandBox::kESModule)
        return m_mjs.run(ctx, src, name, arg_names, args, in_cjs);

    return result_t::make(CALL_E_EXCEPTION).with_message("SandBox: Invalid file format.");
}
}
