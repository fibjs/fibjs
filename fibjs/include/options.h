/*
 * options.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lion
 */

#pragma once

#include "utils.h"
#include "Buffer.h"
#include "ifs/zlib.h"

namespace fibjs {

extern int32_t stack_size;

extern bool g_prof;
extern int32_t g_prof_interval;

extern FILE* g_cov;

// `--cov-exclude=<glob>` (repeatable) and FIBJS_COV_EXCLUDE=<glob>[;<glob>...]:
// the files a pattern matches are left out of the coverage report. Kept as one
// `;` separated list, because a glob may carry a `,` in `{a,b}` itself.
extern exlib::string g_cov_exclude;

extern bool g_tcpdump;
extern bool g_ssldump;
extern bool g_pipedump;

extern exlib::string g_exec_code;
extern bool g_exec_print;

extern bool g_uv_socket;

extern bool g_track_native_object;

extern bool g_openssl_legacy_provider;

extern bool g_use_env_proxy;

// Whether this executable is a single executable application: Runtime::start()
// moves the postject marker into it, and process.features.sea reports it.
extern bool g_is_sea;

// Desktop GUI builds: the dedicated service the main isolate runs on. It is
// created before start() and attached to the process main thread by run_gui(),
// so the whole main isolate executes inside the native event loop. NULL in
// --use-thread mode and on non-desktop builds.
extern exlib::Service* g_main_js_service;

// Every isolate runs all of its JS on one dedicated OS thread, which matches
// the Node.js contract that N-API addons assume (one napi_env == one OS
// thread) and keeps addons that store state in OS thread-local storage working.

struct OptData {
    const char* name;
    int32_t size;
    const char* source;

    void getDate(obj_ptr<Buffer_base>& data) const
    {
        obj_ptr<Buffer_base> zdata = new Buffer(source, size);
        zlib_base::cc_inflate(zdata, -1, data);
    }
};

extern const OptData opt_tools[];

// Embedded JS modules (opt_tools) which are visible to user code, such as
// "stream", "readline" or "timers/promises". Modules stored under these
// prefixes are internal helpers and must not be exposed.
inline bool is_user_opt_tool(const char* name)
{
    return qstrcmp(name, "internal/", 9) != 0 && qstrcmp(name, "opt_tools/", 10) != 0 &&
        qstrcmp(name, "_stream_", 8) != 0;
}
}
