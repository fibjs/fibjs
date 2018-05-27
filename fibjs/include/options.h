/*
 * options.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lion
 */

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "utils.h"
#include "Buffer.h"
#include "ifs/zlib.h"

namespace fibjs {

extern int32_t stack_size;

extern bool g_prof;
extern int32_t g_prof_interval;

extern FILE* g_cov;

extern bool g_tracetcp;

struct OptData {
    const char* name;
    int32_t size;
    const char* source;

    void getDate(obj_ptr<Buffer_base>& data) const
    {
        obj_ptr<Buffer_base> zdata = new Buffer(source, size);
        zlib_base::cc_inflate(zdata, data);
    }
};

extern const OptData opt_tools[];
}

#endif