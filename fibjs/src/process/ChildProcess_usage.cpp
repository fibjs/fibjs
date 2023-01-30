/*
 * ChildProcess.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: lion
 */

#include "object.h"
#include "ChildProcess.h"

#define MICROS_PER_SEC 1e6

#ifdef Darwin
#ifdef iPhone
namespace fibjs {
result_t ChildProcess::usage(v8::Local<v8::Object>& retVal)
{
    return CALL_E_INVALID_CALL;
}
}
#else
#include <libproc.h>
namespace fibjs {
result_t ChildProcess::usage(v8::Local<v8::Object>& retVal)
{
    struct rusage_info_v2 ri;
    int rusage_status = proc_pid_rusage(m_process.pid, RUSAGE_INFO_V2, (rusage_info_t*)&ri);
    if (rusage_status != 0)
        return CHECK_ERROR(LastError());

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(context, isolate->NewString("user"), v8::Number::New(isolate->m_isolate, ri.ri_user_time / 1000)).Check();
    o->Set(context, isolate->NewString("system"), v8::Number::New(isolate->m_isolate, ri.ri_system_time / 1000)).Check();
    o->Set(context, isolate->NewString("rss"), v8::Number::New(isolate->m_isolate, ri.ri_resident_size)).Check();

    retVal = o;

    return 0;
}
}
#endif
#elif defined(Linux)
namespace fibjs {
result_t ChildProcess::usage(v8::Local<v8::Object>& retVal)
{
    char stat_filepath[128];

    snprintf(stat_filepath, sizeof(stat_filepath), "/proc/%d/stat", m_process.pid);
    FILE* fpstat = fopen(stat_filepath, "r");
    if (fpstat == NULL)
        return CHECK_ERROR(LastError());

    long unsigned int utime_ticks = 0;
    long unsigned int stime_ticks = 0;
    long unsigned int rss = 0;

    if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
                       "%lu %lu %*ld %*ld %*d %*d %*d %*d %*u %*lu %ld",
            &utime_ticks, &stime_ticks, &rss)
        == EOF) {
        fclose(fpstat);
        return CHECK_ERROR(LastError());
    }
    fclose(fpstat);

    long herz = sysconf(_SC_CLK_TCK);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(context, isolate->NewString("user"), v8::Number::New(isolate->m_isolate, utime_ticks * MICROS_PER_SEC / herz)).Check();
    o->Set(context, isolate->NewString("system"), v8::Number::New(isolate->m_isolate, stime_ticks * MICROS_PER_SEC / herz)).Check();
    o->Set(context, isolate->NewString("rss"), v8::Number::New(isolate->m_isolate, rss * getpagesize())).Check();

    retVal = o;

    return 0;
}
}
#elif defined(Windows)
#include <psapi.h>

namespace fibjs {
result_t ChildProcess::usage(v8::Local<v8::Object>& retVal)
{
    FILETIME createTime, exitTime, kernelTime, userTime;
    SYSTEMTIME kernelSystemTime, userSystemTime;
    PROCESS_MEMORY_COUNTERS memCounters;
    int ret;

    ret = GetProcessTimes(m_process.process_handle, &createTime, &exitTime, &kernelTime, &userTime);
    if (ret == 0)
        return CHECK_ERROR(LastError());

    ret = FileTimeToSystemTime(&kernelTime, &kernelSystemTime);
    if (ret == 0)
        return CHECK_ERROR(LastError());

    ret = FileTimeToSystemTime(&userTime, &userSystemTime);
    if (ret == 0)
        return CHECK_ERROR(LastError());

    ret = GetProcessMemoryInfo(m_process.process_handle, &memCounters, sizeof(memCounters));
    if (ret == 0)
        return CHECK_ERROR(LastError());

    double utime_ticks = MICROS_PER_SEC * (userSystemTime.wHour * 3600 + userSystemTime.wMinute * 60 + userSystemTime.wSecond)
        + userSystemTime.wMilliseconds * 1000;
    double stime_ticks = MICROS_PER_SEC * (kernelSystemTime.wHour * 3600 + kernelSystemTime.wMinute * 60 + kernelSystemTime.wSecond)
        + kernelSystemTime.wMilliseconds * 1000;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(context, isolate->NewString("user"), v8::Number::New(isolate->m_isolate, utime_ticks)).Check();
    o->Set(context, isolate->NewString("system"), v8::Number::New(isolate->m_isolate, stime_ticks)).Check();
    o->Set(context, isolate->NewString("rss"), v8::Number::New(isolate->m_isolate, (double)memCounters.PeakWorkingSetSize)).Check();

    retVal = o;

    return 0;
}
}
#endif