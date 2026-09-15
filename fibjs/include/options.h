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

extern bool g_tcpdump;
extern bool g_ssldump;
extern bool g_pipedump;

extern exlib::string g_exec_code;

extern bool g_uv_socket;

extern bool g_track_native_object;

extern bool g_openssl_legacy_provider;

extern bool g_use_env_proxy;

// Every isolate runs all of its JS on one dedicated OS thread by default, which
// matches the Node.js contract that N-API addons assume (one napi_env == one OS
// thread) and keeps addons that store state in OS thread-local storage working.
//
// Pass --no-js-thread-affinity to schedule the fibers of one isolate across the
// shared thread pool again: more JS parallelism for CPU bound fibers, but
// thread-local state inside an addon is not reliable.
extern bool g_js_thread_affinity;

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
}
