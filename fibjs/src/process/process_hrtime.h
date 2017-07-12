/*
 * process_hrtime.h
 *
 *  Created on: Jul 12, 2017
 *      Author: lion
 */
#include <exlib/include/osconfig.h>

#define NANOS_PER_SEC 1000000000

#ifdef _WIN32

#include <windows.h>

namespace fibjs {

inline uint64_t _hrtime()
{
    static double hrtime_interval_ = 0;

    /* If the performance interval is zero, there's no support. */
    if (hrtime_interval_ == 0) {
        LARGE_INTEGER perf_frequency;
        if (QueryPerformanceFrequency(&perf_frequency))
            hrtime_interval_ = 1.0 / perf_frequency.QuadPart;
        else
            hrtime_interval_ = 0;

        if (hrtime_interval_ == 0)
            return 0;
    }

    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter))
        return 0;

    /* Because we have no guarantee about the order of magnitude of the
   * performance counter interval, integer math could cause this computation
   * to overflow. Therefore we resort to floating point math.
   */
    return (uint64_t)((double)counter.QuadPart * hrtime_interval_ * NANOS_PER_SEC);
}
}

#elif defined(Darwin)

#include "object.h"
#include "ifs/process.h"
#include <mach/mach_time.h>

namespace fibjs {

inline uint64_t _hrtime()
{
    static mach_timebase_info_data_t info;

    if ((info.numer == 0 || info.denom == 0)
        && mach_timebase_info(&info) != KERN_SUCCESS)
        abort();

    return mach_absolute_time() * info.numer / info.denom;
}
}
#else

#include <time.h>

namespace fibjs {

inline uint64_t _hrtime()
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t))
        return 0;

    return t.tv_sec * NANOS_PER_SEC + t.tv_nsec;
}
}
#endif
