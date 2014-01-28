/*
 * os_freebsd.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <osconfig.h>

#ifdef FreeBSD

#include "ifs/os.h"
#include <kvm.h>
#include <fcntl.h>
#include <paths.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <vm/vm_param.h>
#include <stdlib.h>

#undef NANOSEC
#define NANOSEC ((uint64_t) 1e9)

#ifndef CPUSTATES
# define CPUSTATES 5U
#endif

#ifndef CP_USER
# define CP_USER 0
# define CP_NICE 1
# define CP_SYS 2
# define CP_IDLE 3
# define CP_INTR 4
#endif


namespace fibjs
{

result_t os_base::uptime(double &retVal)
{
    time_t now;
    struct timeval info;
    size_t size = sizeof(info);
    static int which[] = { CTL_KERN, KERN_BOOTTIME };

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return LastError();

    now = ::time(NULL);

    retVal = (double) (now - info.tv_sec);
    return 0;
}

result_t os_base::loadavg(v8::Local<v8::Array> &retVal)
{
    double avg[3] = { 0, 0, 0 };

    struct loadavg info;
    size_t size = sizeof(info);
    int which[] = { CTL_VM, VM_LOADAVG };

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return LastError();

    avg[0] = (double) info.ldavg[0] / info.fscale;
    avg[1] = (double) info.ldavg[1] / info.fscale;
    avg[2] = (double) info.ldavg[2] / info.fscale;

    retVal = v8::Array::New(isolate, 3);
    retVal->Set(0, v8::Number::New(isolate, avg[0]));
    retVal->Set(1, v8::Number::New(isolate, avg[1]));
    retVal->Set(2, v8::Number::New(isolate, avg[2]));

    return 0;
}

result_t os_base::totalmem(int64_t &retVal)
{
    uint64_t info;
    int which[] = { CTL_HW, HW_PHYSMEM };
    size_t size = sizeof(info);

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return LastError();

    retVal = info;

    return 0;
}

result_t os_base::freemem(int64_t &retVal)
{
    int freecount;
    size_t size = sizeof(freecount);

    if (sysctlbyname("vm.stats.vm.v_free_count", &freecount, &size, NULL, 0)
            == -1)
        return LastError();

    retVal = (int64_t) freecount * sysconf(_SC_PAGESIZE);

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

    int numcpus;
    size_t size;

    size = sizeof(numcpus);
    if (sysctlbyname("hw.ncpu", &numcpus, &size, NULL, 0) < 0)
        return LastError();

    retVal = cpus = numcpus;

    return 0;
}

result_t os_base::CPUInfo(v8::Local<v8::Array> &retVal)
{
    retVal = v8::Array::New(isolate);

    v8::Local<v8::Object> cpuinfo;
    v8::Local<v8::Object> cputimes;

    unsigned int ticks = (unsigned int) sysconf(_SC_CLK_TCK), multiplier =
                             ((uint64_t) 1000L / ticks), cpuspeed, maxcpus, cur = 0;
    char model[512];
    long *cp_times;
    int numcpus;
    size_t size;
    int i;

    size = sizeof(model);
    if (sysctlbyname("hw.model", &model, &size, NULL, 0) < 0)
        return LastError();

    size = sizeof(numcpus);
    if (sysctlbyname("hw.ncpu", &numcpus, &size, NULL, 0) < 0)
        return LastError();

    size = sizeof(cpuspeed);
    if (sysctlbyname("hw.clockrate", &cpuspeed, &size, NULL, 0) < 0)
        return LastError();

    size = sizeof(maxcpus);
#ifdef __DragonFly__
    if (sysctlbyname("hw.ncpu", &maxcpus, &size, NULL, 0) < 0)
        return LastError();
#else
    if (sysctlbyname("kern.smp.maxcpus", &maxcpus, &size, NULL, 0) < 0)
        return LastError();
#endif

    size = maxcpus * CPUSTATES * sizeof(long);

    cp_times = (long int *) malloc(size);
    if (cp_times == NULL)
        return LastError();

    if (sysctlbyname("kern.cp_times", cp_times, &size, NULL, 0) < 0)
    {
        free(cp_times);
        return LastError();
    }

    for (i = 0; i < numcpus; i++)
    {
        cpuinfo = v8::Object::New(isolate);
        cputimes = v8::Object::New(isolate);
        cputimes->Set(
            v8::String::NewFromUtf8(isolate, "user"),
            v8::Number::New(isolate,
                            (uint64_t)(cp_times[CP_USER + cur]) * multiplier));
        cputimes->Set(
            v8::String::NewFromUtf8(isolate, "nice"),
            v8::Number::New(isolate,
                            (uint64_t)(cp_times[CP_NICE + cur]) * multiplier));
        cputimes->Set(
            v8::String::NewFromUtf8(isolate, "sys"),
            v8::Number::New(isolate, (uint64_t)(cp_times[CP_SYS + cur]) * multiplier));
        cputimes->Set(
            v8::String::NewFromUtf8(isolate, "idle"),
            v8::Number::New(isolate,
                            (uint64_t)(cp_times[CP_IDLE + cur]) * multiplier));
        cputimes->Set(
            v8::String::NewFromUtf8(isolate, "irq"),
            v8::Number::New(isolate,
                            (uint64_t)(cp_times[CP_INTR + cur]) * multiplier));

        cpuinfo->Set(v8::String::NewFromUtf8(isolate, "model"), v8::String::NewFromUtf8(isolate, model));
        cpuinfo->Set(v8::String::NewFromUtf8(isolate, "speed"), v8::Number::New(isolate, cpuspeed));

        cpuinfo->Set(v8::String::NewFromUtf8(isolate, "times"), cputimes);
        retVal->Set(i, cpuinfo);

        cur += CPUSTATES;
    }

    free(cp_times);

    return 0;
}

result_t os_base::get_execPath(std::string &retVal)
{
    char exeName[1024] = "";
    size_t size = sizeof(exeName);

    int mib[4];

#ifdef __DragonFly__
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ARGS;
    mib[3] = getpid();
#else
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
#endif

    if (sysctl(mib, 4, exeName, &size, NULL, 0) < 0)
        return LastError();

    retVal = exeName;
    return 0;
}

int g_obj_refs;
result_t os_base::memoryUsage(v8::Local<v8::Object> &retVal)
{
    size_t rss;

    kvm_t *kd = NULL;
    struct kinfo_proc *kinfo = NULL;
    pid_t pid;
    int nprocs;
    size_t page_size = getpagesize();

    pid = getpid();

    kd = kvm_open(NULL, _PATH_DEVNULL, NULL, O_RDONLY, "kvm_open");
    if (kd == NULL)
        return LastError();

    kinfo = kvm_getprocs(kd, KERN_PROC_PID, pid, &nprocs);
    if (kinfo == NULL)
    {
        kvm_close(kd);
        return LastError();
    }

#ifdef __DragonFly__
    rss = kinfo->kp_vm_rssize * page_size;
#else
    rss = kinfo->ki_rssize * page_size;
#endif
    kvm_close(kd);

    v8::Local<v8::Object> info = v8::Object::New(isolate);

    v8::HeapStatistics v8_heap_stats;
    isolate->GetHeapStatistics(&v8_heap_stats);
    info->Set(v8::String::NewFromUtf8(isolate, "rss"), v8::Integer::New(isolate, (int32_t)rss));
    info->Set(v8::String::NewFromUtf8(isolate, "heapTotal"),
              v8::Integer::New(isolate, (int32_t)v8_heap_stats.total_heap_size()));
    info->Set(v8::String::NewFromUtf8(isolate, "heapUsed"),
              v8::Integer::New(isolate, (int32_t)v8_heap_stats.used_heap_size()));
    info->Set(v8::String::NewFromUtf8(isolate, "nativeObjects"),
              v8::Integer::New(isolate, (int32_t)g_obj_refs));

    retVal = info;

    return 0;
}

}

#endif
