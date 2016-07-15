/*
 * os_freebsd.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifdef FreeBSD

#include "ifs/os.h"
#include <kvm.h>
#include <dlfcn.h>
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
    static int32_t which[] = { CTL_KERN, KERN_BOOTTIME };

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    now = ::time(NULL);

    retVal = (double) (now - info.tv_sec);
    return 0;
}

result_t os_base::loadavg(v8::Local<v8::Array> &retVal)
{
    double avg[3] = { 0, 0, 0 };
    Isolate* isolate = Isolate::current();

    struct loadavg info;
    size_t size = sizeof(info);
    int32_t which[] = { CTL_VM, VM_LOADAVG };

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    avg[0] = (double) info.ldavg[0] / info.fscale;
    avg[1] = (double) info.ldavg[1] / info.fscale;
    avg[2] = (double) info.ldavg[2] / info.fscale;

    retVal = v8::Array::New(isolate->m_isolate, 3);
    retVal->Set(0, v8::Number::New(isolate->m_isolate, avg[0]));
    retVal->Set(1, v8::Number::New(isolate->m_isolate, avg[1]));
    retVal->Set(2, v8::Number::New(isolate->m_isolate, avg[2]));

    return 0;
}

result_t os_base::totalmem(int64_t &retVal)
{
    uint64_t info;
    int32_t which[] = { CTL_HW, HW_PHYSMEM };
    size_t size = sizeof(info);

    if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    retVal = info;

    return 0;
}

result_t os_base::freemem(int64_t &retVal)
{
    int32_t freecount;
    size_t size = sizeof(freecount);

    if (sysctlbyname("vm.stats.vm.v_free_count", &freecount, &size, NULL, 0)
            == -1)
        return CHECK_ERROR(LastError());

    retVal = (int64_t) freecount * sysconf(_SC_PAGESIZE);

    return 0;
}

result_t os_base::CPUs(int32_t &retVal)
{
    static int32_t cpus = 0;

    if (cpus > 0)
    {
        retVal = cpus;
        return 0;
    }

    int32_t numcpus;
    size_t size;

    size = sizeof(numcpus);
    if (sysctlbyname("hw.ncpu", &numcpus, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    retVal = cpus = numcpus;

    return 0;
}

result_t os_base::CPUInfo(v8::Local<v8::Array> &retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = v8::Array::New(isolate->m_isolate);

    v8::Local<v8::Object> cpuinfo;
    v8::Local<v8::Object> cputimes;

    uint32_t ticks = (uint32_t) sysconf(_SC_CLK_TCK), multiplier =
                         ((uint64_t) 1000L / ticks), cpuspeed, maxcpus, cur = 0;
    char model[512];
    long *cp_times;
    int32_t numcpus;
    size_t size;
    int32_t i;

    size = sizeof(model);
    if (sysctlbyname("hw.model", &model, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    size = sizeof(numcpus);
    if (sysctlbyname("hw.ncpu", &numcpus, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    size = sizeof(cpuspeed);
    if (sysctlbyname("hw.clockrate", &cpuspeed, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());

    size = sizeof(maxcpus);
#ifdef __DragonFly__
    if (sysctlbyname("hw.ncpu", &maxcpus, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());
#else
    if (sysctlbyname("kern.smp.maxcpus", &maxcpus, &size, NULL, 0) < 0)
        return CHECK_ERROR(LastError());
#endif

    size = maxcpus * CPUSTATES * sizeof(long);

    cp_times = (long int *) malloc(size);
    if (cp_times == NULL)
        return CHECK_ERROR(LastError());

    if (sysctlbyname("kern.cp_times", cp_times, &size, NULL, 0) < 0)
    {
        free(cp_times);
        return CHECK_ERROR(LastError());
    }

    for (i = 0; i < numcpus; i++)
    {
        cpuinfo = v8::Object::New(isolate->m_isolate);
        cputimes = v8::Object::New(isolate->m_isolate);
        cputimes->Set(
            isolate->NewFromUtf8("user"),
            v8::Number::New(isolate->m_isolate,
                            (uint64_t)(cp_times[CP_USER + cur]) * multiplier));
        cputimes->Set(
            isolate->NewFromUtf8("nice"),
            v8::Number::New(isolate->m_isolate,
                            (uint64_t)(cp_times[CP_NICE + cur]) * multiplier));
        cputimes->Set(
            isolate->NewFromUtf8("sys"),
            v8::Number::New(isolate->m_isolate, (uint64_t)(cp_times[CP_SYS + cur]) * multiplier));
        cputimes->Set(
            isolate->NewFromUtf8("idle"),
            v8::Number::New(isolate->m_isolate,
                            (uint64_t)(cp_times[CP_IDLE + cur]) * multiplier));
        cputimes->Set(
            isolate->NewFromUtf8("irq"),
            v8::Number::New(isolate->m_isolate,
                            (uint64_t)(cp_times[CP_INTR + cur]) * multiplier));

        cpuinfo->Set(isolate->NewFromUtf8("model"), isolate->NewFromUtf8(model));
        cpuinfo->Set(isolate->NewFromUtf8("speed"), v8::Number::New(isolate->m_isolate, cpuspeed));

        cpuinfo->Set(isolate->NewFromUtf8("times"), cputimes);
        retVal->Set(i, cpuinfo);

        cur += CPUSTATES;
    }

    free(cp_times);

    return 0;
}

result_t os_base::get_execPath(exlib::string &retVal)
{
    char exeName[1024] = "";
    size_t size = sizeof(exeName);

    int32_t mib[4];

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
        return CHECK_ERROR(LastError());

    retVal = exeName;
    return 0;
}

result_t os_base::memoryUsage(v8::Local<v8::Object> &retVal)
{
    size_t rss;

    kvm_t *kd = NULL;
    struct kinfo_proc *kinfo = NULL;
    pid_t pid;
    int32_t nprocs;
    size_t page_size = getpagesize();

    static bool _init = false;
    static kvm_t *(*_kvm_open)(char *, const char *, char *, int32_t, const char *);
    static void (*_kvm_close)(kvm_t *);
    static struct kinfo_proc* (*_kvm_getprocs)(kvm_t *, int32_t, int32_t, int32_t *);

    if (!_init)
    {
        void *handle = dlopen("libkvm.so", RTLD_LAZY);

        if (handle)
        {
            _kvm_open = (kvm_t * (*)(char *, const char *, char *, int32_t, const char *))dlsym(handle, "kvm_open");
            _kvm_getprocs = (struct kinfo_proc * (*)(kvm_t *, int32_t, int32_t, int32_t *))dlsym(handle, "kvm_getprocs");
            _kvm_close = (void (*)(kvm_t *))dlsym(handle, "kvm_close");
        }
    }

    if (!_kvm_open || !_kvm_getprocs || !_kvm_close)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    pid = getpid();

    kd = _kvm_open(NULL, _PATH_DEVNULL, NULL, O_RDONLY, "kvm_open");
    if (kd == NULL)
        return CHECK_ERROR(LastError());

    kinfo = _kvm_getprocs(kd, KERN_PROC_PID, pid, &nprocs);
    if (kinfo == NULL)
    {
        _kvm_close(kd);
        return CHECK_ERROR(LastError());
    }

#ifdef __DragonFly__
    rss = kinfo->kp_vm_rssize * page_size;
#else
    rss = kinfo->ki_rssize * page_size;
#endif
    _kvm_close(kd);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> info = v8::Object::New(isolate->m_isolate);

    v8::HeapStatistics v8_heap_stats;
    isolate->m_isolate->GetHeapStatistics(&v8_heap_stats);
    info->Set(isolate->NewFromUtf8("rss"), v8::Number::New(isolate->m_isolate, (double)rss));
    info->Set(isolate->NewFromUtf8("heapTotal"),
              v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.total_heap_size()));
    info->Set(isolate->NewFromUtf8("heapUsed"),
              v8::Number::New(isolate->m_isolate, (double)v8_heap_stats.used_heap_size()));

    v8::Local<v8::Object> objs;
    object_base::class_info().dump(objs);
    info->Set(isolate->NewFromUtf8("nativeObjects"), objs);

    retVal = info;

    return 0;
}

}

#endif
