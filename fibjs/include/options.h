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

extern exlib::string g_exec_code;

extern bool g_uv_socket;

extern bool g_track_native_object;

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
