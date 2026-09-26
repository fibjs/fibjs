/*
 * ts_loader.cpp
 *
 *  Created on: Sep 19, 2024
 *      Author: lion
 */

#include "object.h"
#include "loaders.h"

namespace fibjs {

result_t ts_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs)
{
    result_t hr;
    SandBox::ModuleTypeInfo info;

    hr = ctx->m_sb->resolveModuleType(name, info);
    if (hr)
        return hr;

    if (info.type == SandBox::kCommonJS) {
        hr = m_cts.run(ctx, src, name, arg_names, args, in_cjs);
        if (hr >= 0)
            return hr;

        // A `.ts` file is as ambiguous as a `.js` one: neither the extension nor
        // package.json says which module system it wants, so `import`/`export`
        // has to be recognised and the file retried as ESM. Without this every
        // TypeScript file written as an ES module fails to load unless the
        // nearest package.json happens to say "type": "module" - which is what
        // Node.js does too, but Node reaches the same conclusion by itself.
        if (hr == CALL_E_EXCEPTION) {
            exlib::string err_msg = Runtime::errMessage();

            // Detection is only for files nothing else has classified. When
            // package.json says "type": "commonjs", `export` in the file is an
            // error rather than a module to discover - Node.js draws the same
            // line and refuses to reparse such a file.
            if (!info.explicitType && shouldRetryAsESM(err_msg, src, name, true)) {
                if (in_cjs)
                    return m_mts.run(ctx, src, name, arg_names, args, in_cjs);

                // Loaded through the CJS path from an ES module importer: leave
                // the sentinel behind so esm_importer::resove_module() retries
                // this file as ESM. Same contract as js_Loader.
                return CHECK_ERROR(Runtime::setError(
                    "SandBox: ESM syntax detected; retry as ES module."));
            }

            // Not ESM syntax: restore the original error, which is the one the
            // author needs to see (a strip error, a genuine syntax error, ...).
            return CHECK_ERROR(Runtime::setError(err_msg));
        }

        return hr;
    } else if (info.type == SandBox::kESModule)
        return m_mts.run(ctx, src, name, arg_names, args, in_cjs);

    return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));
}
}