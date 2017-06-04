/*
 * os_linux.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifdef Linux

#include "object.h"
#include "ifs/os.h"
#include <dlfcn.h>
#include <sys/sysinfo.h>
#include <sys/param.h>
#include <string.h>

#ifndef CLOCK_BOOTTIME
#define CLOCK_BOOTTIME 7
#endif

namespace fibjs {

result_t os_base::uptime(double& retVal)
{
    static volatile int32_t no_clock_boottime;
    struct timespec now;
    int32_t r;
    int32_t (*fngettime)(clockid_t, struct timespec*);

    void* handle = dlopen("librt.so", RTLD_LAZY);
    if (!handle)
        return CHECK_ERROR(LastError());

    fngettime = (int32_t(*)(clockid_t, struct timespec*))dlsym(handle, "clock_gettime");
    if (!fngettime) {
        dlclose(handle);
        return CHECK_ERROR(LastError());
    }

    if (no_clock_boottime) {
    retry:
        r = fngettime(CLOCK_MONOTONIC, &now);
    } else if ((r = fngettime(CLOCK_BOOTTIME, &now)) && errno == EINVAL) {
        no_clock_boottime = 1;
        goto retry;
    }

    dlclose(handle);

    if (r)
        return CHECK_ERROR(LastError());

    retVal = now.tv_sec;
    retVal += (double)now.tv_nsec / 1000000000.0;

    return 0;
}

result_t os_base::loadavg(v8::Local<v8::Array>& retVal)
{
    double avg[3] = { 0, 0, 0 };

    struct sysinfo info;

    if (sysinfo(&info) < 0)
        return CHECK_ERROR(LastError());

    avg[0] = (double)info.loads[0] / 65536.0;
    avg[1] = (double)info.loads[1] / 65536.0;
    avg[2] = (double)info.loads[2] / 65536.0;

    Isolate* isolate = Isolate::current();
    retVal = v8::Array::New(isolate->m_isolate, 3);
    retVal->Set(0, v8::Number::New(isolate->m_isolate, avg[0]));
    retVal->Set(1, v8::Number::New(isolate->m_isolate, avg[1]));
    retVal->Set(2, v8::Number::New(isolate->m_isolate, avg[2]));

    return 0;
}

result_t os_base::totalmem(int64_t& retVal)
{
    retVal = sysconf(_SC_PAGESIZE) * sysconf(_SC_PHYS_PAGES);
    return 0;
}

result_t os_base::freemem(int64_t& retVal)
{
    retVal = sysconf(_SC_PAGESIZE) * sysconf(_SC_AVPHYS_PAGES);
    return 0;
}

result_t os_base::cpuNumbers(int32_t& retVal)
{
    static int32_t cpus = 0;

    if (cpus > 0) {
        retVal = cpus;
        return 0;
    }

    int32_t numcpus = 0;
    char line[512];
    FILE* fpModel = fopen("/proc/cpuinfo", "r");

    if (fpModel) {
        while (fgets(line, 511, fpModel) != NULL)
            if (strncmp(line, "processor", 9) == 0)
                numcpus++;
        fclose(fpModel);

        retVal = cpus = numcpus;
    } else
        return CHECK_ERROR(LastError());

    return 0;
}

result_t os_base::cpus(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Array::New(isolate->m_isolate);

    v8::Local<v8::Object> cpuinfo;
    v8::Local<v8::Object> cputimes;
    uint32_t ticks = (uint32_t)sysconf(_SC_CLK_TCK), multiplier = ((uint64_t)1000L / ticks), cpuspeed;
    int32_t numcpus = 0, i = 0;
    unsigned long long ticks_user, ticks_sys, ticks_idle, ticks_nice,
        ticks_intr;
    char line[512], speedPath[256], model[512] = "";
    FILE* fpStat = fopen("/proc/stat", "r");
    FILE* fpModel = fopen("/proc/cpuinfo", "r");
    FILE* fpSpeed;

    if (fpModel) {
        while (fgets(line, 511, fpModel) != NULL) {
            if (strncmp(line, "processor", 9) == 0)
                numcpus++;
            else if (strncmp(line, "model name", 10) == 0) {
                if (numcpus == 1) {
                    char* p = strchr(line, ':') + 2;
                    strcpy(model, p);
                    model[strlen(model) - 1] = 0;
                }
            } else if (strncmp(line, "cpu MHz", 7) == 0) {
                if (numcpus == 1)
                    sscanf(line, "%*s %*s : %u", &cpuspeed);
            }
        }
        fclose(fpModel);
    }

    if (fpStat) {
        while (fgets(line, 511, fpStat) != NULL) {
            if (strncmp(line, "cpu ", 4) == 0)
                continue;
            else if (strncmp(line, "cpu", 3) != 0)
                break;

            sscanf(line, "%*s %llu %llu %llu %llu %*llu %llu", &ticks_user,
                &ticks_nice, &ticks_sys, &ticks_idle, &ticks_intr);
            snprintf(speedPath, sizeof(speedPath),
                "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_max_freq",
                i);

            fpSpeed = fopen(speedPath, "r");

            if (fpSpeed) {
                if (fgets(line, 511, fpSpeed) != NULL) {
                    sscanf(line, "%u", &cpuspeed);
                    cpuspeed /= 1000;
                }
                fclose(fpSpeed);
            }

            cpuinfo = v8::Object::New(isolate->m_isolate);
            cputimes = v8::Object::New(isolate->m_isolate);
            cputimes->Set(isolate->NewFromUtf8("user"),
                v8::Number::New(isolate->m_isolate, ticks_user * multiplier));
            cputimes->Set(isolate->NewFromUtf8("nice"),
                v8::Number::New(isolate->m_isolate, ticks_nice * multiplier));
            cputimes->Set(isolate->NewFromUtf8("sys"),
                v8::Number::New(isolate->m_isolate, ticks_sys * multiplier));
            cputimes->Set(isolate->NewFromUtf8("idle"),
                v8::Number::New(isolate->m_isolate, ticks_idle * multiplier));
            cputimes->Set(isolate->NewFromUtf8("irq"),
                v8::Number::New(isolate->m_isolate, ticks_intr * multiplier));

            if (model[0])
                cpuinfo->Set(isolate->NewFromUtf8("model"), isolate->NewFromUtf8(model));
            cpuinfo->Set(isolate->NewFromUtf8("speed"), v8::Number::New(isolate->m_isolate, cpuspeed));

            cpuinfo->Set(isolate->NewFromUtf8("times"), cputimes);
            retVal->Set(i++, cpuinfo);
        }
        fclose(fpStat);
    }

    return 0;
}

result_t os_base::get_execPath(exlib::string& retVal)
{
    size_t linksize = 256;
    char exeName[256] = { 0 };

    if (readlink("/proc/self/exe", exeName, linksize) == -1)
        return CHECK_ERROR(LastError());

    retVal = exeName;
    return 0;
}

result_t os_base::memoryUsage(v8::Local<v8::Object>& retVal)
{
    size_t rss = 0;

    FILE* f;
    int32_t itmp;
    char ctmp;
    uint32_t utmp;
    size_t page_size = getpagesize();
    char* cbuf;
    int32_t foundExeEnd;
    static char buf[MAXPATHLEN + 1];

    f = fopen("/proc/self/stat", "r");
    if (!f)
        return CHECK_ERROR(LastError());

    /* PID */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Exec file */
    cbuf = buf;
    foundExeEnd = 0;
    if (fscanf(f, "%c", cbuf++) == 0)
        goto error;

    while (1) {
        if (fscanf(f, "%c", cbuf) == 0)
            goto error;
        if (*cbuf == ')') {
            foundExeEnd = 1;
        } else if (foundExeEnd && *cbuf == ' ') {
            *cbuf = 0;
            break;
        }

        cbuf++;
    }
    /* State */
    if (fscanf(f, "%c ", &ctmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Parent process */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Process group */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Session id */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* TTY */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* TTY owner process group */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Flags */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Minor faults (no memory page) */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Minor faults, children */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Major faults (memory page faults) */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Major faults, children */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* utime */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* stime */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* utime, children */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* stime, children */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* jiffies remaining in current time slice */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* 'nice' value */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* jiffies until next timeout */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* jiffies until next SIGALRM */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* start time (jiffies since system boot) */
    if (fscanf(f, "%d ", &itmp) == 0)
        goto error;
    /* coverity[secure_coding] */

    /* Virtual memory size */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */

    /* Resident set size */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    rss = (size_t)utmp * page_size;

    /* rlim */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Start of text */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* End of text */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
    /* coverity[secure_coding] */
    /* Start of stack */
    if (fscanf(f, "%u ", &utmp) == 0)
        goto error;
/* coverity[secure_coding] */

error:
    fclose(f);

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
