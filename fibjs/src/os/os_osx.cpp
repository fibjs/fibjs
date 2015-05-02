/*
 * os_osx.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifdef MacOS

#include "ifs/os.h"
#include <mach/mach.h>
#include <mach-o/dyld.h>
#include <sys/sysctl.h>

namespace fibjs
{

result_t os_base::uptime(double &retVal)
{
    time_t now;
    struct timeval info;
    size_t size = sizeof(info);
    static int which[] =
    { CTL_KERN, KERN_BOOTTIME };

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    now = ::time(NULL);

    retVal = (double) (now - info.tv_sec);
    return 0;
}

result_t os_base::loadavg(v8::Local<v8::Array> &retVal)
{
    Isolate &isolate = Isolate::now();
    double avg[3] =
    { 0, 0, 0 };

    struct loadavg info;
    size_t size = sizeof(info);
    int which[] =
    { CTL_VM, VM_LOADAVG };

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    avg[0] = (double) info.ldavg[0] / info.fscale;
    avg[1] = (double) info.ldavg[1] / info.fscale;
    avg[2] = (double) info.ldavg[2] / info.fscale;

    retVal = v8::Array::New(isolate.isolate, 3);
    retVal->Set(0, v8::Number::New(isolate.isolate, avg[0]));
    retVal->Set(1, v8::Number::New(isolate.isolate, avg[1]));
    retVal->Set(2, v8::Number::New(isolate.isolate, avg[2]));

    return 0;
}

result_t os_base::totalmem(int64_t &retVal)
{
    uint64_t info;
    int which[] =
    { CTL_HW, HW_MEMSIZE };
    size_t size = sizeof(info);

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    retVal = info;

    return 0;
}

result_t os_base::freemem(int64_t &retVal)
{
    vm_statistics_data_t info;
    mach_msg_type_number_t count = sizeof(info) / sizeof(integer_t);

    if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t) & info,
                        &count) != KERN_SUCCESS)
        return CHECK_ERROR(LastError());

    retVal = info.free_count * sysconf(_SC_PAGESIZE);
    return 0;
}

result_t os_base::CPUs(int32_t &retVal)
{
    static int cpus = 0;

    if (cpus > 0)
    {
        retVal = cpus;
        return 0;
    }

    natural_t numcpus;
    mach_msg_type_number_t count;
    processor_cpu_load_info_data_t *info;
    if (host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numcpus,
                            reinterpret_cast<processor_info_array_t *>(&info),
                            &count) != KERN_SUCCESS)
        return CHECK_ERROR(LastError());

    vm_deallocate(mach_task_self(), (vm_address_t) info, count);

    retVal = cpus = numcpus;

    return 0;
}

result_t os_base::CPUInfo(v8::Local<v8::Array> &retVal)
{
    Isolate &isolate = Isolate::now();
    retVal = v8::Array::New(isolate.isolate);

    v8::Local<v8::Object> cpuinfo;
    v8::Local<v8::Object> cputimes;
    unsigned int ticks = (unsigned int) sysconf(_SC_CLK_TCK), multiplier =
                             ((uint64_t) 1000L / ticks);
    char model[512];
    uint64_t cpuspeed;
    size_t size;

    size = sizeof(model);
    if (sysctlbyname("hw.model", &model, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    size = sizeof(cpuspeed);
    if (sysctlbyname("hw.cpufrequency", &cpuspeed, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    natural_t numcpus;
    mach_msg_type_number_t count;
    processor_cpu_load_info_data_t *info;
    if (host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numcpus,
                            reinterpret_cast<processor_info_array_t *>(&info),
                            &count) != KERN_SUCCESS)
        return CHECK_ERROR(LastError());

    retVal = v8::Array::New(isolate.isolate, numcpus);
    for (unsigned int i = 0; i < numcpus; i++)
    {
        cpuinfo = v8::Object::New(isolate.isolate);
        cputimes = v8::Object::New(isolate.isolate);
        cputimes->Set(v8::String::NewFromUtf8(isolate.isolate, "user"),
                      v8::Number::New(isolate.isolate, (uint64_t)(info[i].cpu_ticks[0]) * multiplier));
        cputimes->Set(v8::String::NewFromUtf8(isolate.isolate, "nice"),
                      v8::Number::New(isolate.isolate, (uint64_t)(info[i].cpu_ticks[3]) * multiplier));
        cputimes->Set(v8::String::NewFromUtf8(isolate.isolate, "sys"),
                      v8::Number::New(isolate.isolate, (uint64_t)(info[i].cpu_ticks[1]) * multiplier));
        cputimes->Set(v8::String::NewFromUtf8(isolate.isolate, "idle"),
                      v8::Number::New(isolate.isolate, (uint64_t)(info[i].cpu_ticks[2]) * multiplier));
        cputimes->Set(v8::String::NewFromUtf8(isolate.isolate, "irq"), v8::Number::New(isolate.isolate, 0));

        cpuinfo->Set(v8::String::NewFromUtf8(isolate.isolate, "model"), v8::String::NewFromUtf8(isolate.isolate, model));
        cpuinfo->Set(v8::String::NewFromUtf8(isolate.isolate, "speed"),
                     v8::Number::New(isolate.isolate, cpuspeed / 1000000));

        cpuinfo->Set(v8::String::NewFromUtf8(isolate.isolate, "times"), cputimes);
        retVal->Set(i, cpuinfo);
    }
    vm_deallocate(mach_task_self(), (vm_address_t) info, count);

    return 0;
}

result_t os_base::get_execPath(std::string &retVal)
{
    char exeName[1024] = "";
    uint32_t size = sizeof(exeName);

    _NSGetExecutablePath(exeName, &size);

    retVal = exeName;
    return 0;
}

result_t os_base::memoryUsage(v8::Local<v8::Object> &retVal)
{
    size_t rss = 0;

    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t) &t_info,
                  &t_info_count) != KERN_SUCCESS)
        return CHECK_ERROR(LastError());

    rss = t_info.resident_size;
    Isolate &isolate = Isolate::now();

    v8::Local<v8::Object> info = v8::Object::New(isolate.isolate);

    v8::HeapStatistics v8_heap_stats;
    isolate.isolate->GetHeapStatistics(&v8_heap_stats);
    info->Set(v8::String::NewFromUtf8(isolate.isolate, "rss"), v8::Integer::New(isolate.isolate, (int32_t)rss));
    info->Set(v8::String::NewFromUtf8(isolate.isolate, "heapTotal"),
              v8::Integer::New(isolate.isolate, (int32_t)v8_heap_stats.total_heap_size()));
    info->Set(v8::String::NewFromUtf8(isolate.isolate, "heapUsed"),
              v8::Integer::New(isolate.isolate, (int32_t)v8_heap_stats.used_heap_size()));

    v8::Local<v8::Object> objs;
    object_base::class_info().dump(objs);
    info->Set(v8::String::NewFromUtf8(isolate.isolate, "nativeObjects"), objs);

    retVal = info;

    return 0;
}

}

#endif

