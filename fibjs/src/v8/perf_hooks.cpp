/*
 * perf_hooks.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/perf_hooks.h"
#include <uv/include/uv.h>

namespace fibjs {

DECLARE_MODULE(perf_hooks);

static uint64_t s_startup = uv_hrtime();

result_t performance_base::now(double& retVal)
{
    retVal = (double)(uv_hrtime() - s_startup) / 1000000.0;
    return 0;
}

}