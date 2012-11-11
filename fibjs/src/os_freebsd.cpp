/*
 * os_freebsd.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <osconfig.h>

#ifdef FreeBSD

#include "ifs/os.h"
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <vm/vm_param.h>

namespace fibjs {

result_t os_base::uptime(double& retVal) {
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

result_t os_base::loadavg(v8::Handle<v8::Array>& retVal) {
	double avg[3] = { 0, 0, 0 };

	struct loadavg info;
	size_t size = sizeof(info);
	int which[] = { CTL_VM, VM_LOADAVG };

	if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
		return LastError();

	avg[0] = (double) info.ldavg[0] / info.fscale;
	avg[1] = (double) info.ldavg[1] / info.fscale;
	avg[2] = (double) info.ldavg[2] / info.fscale;

	retVal = v8::Array::New(3);
	retVal->Set(0, v8::Number::New(avg[0]));
	retVal->Set(1, v8::Number::New(avg[1]));
	retVal->Set(2, v8::Number::New(avg[2]));

	return 0;
}

result_t os_base::totalmem(int64_t& retVal) {
	uint64_t info;
	int which[] = { CTL_HW, HW_PHYSMEM };
	size_t size = sizeof(info);

	if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
		return LastError();

	retVal = info;

	return 0;
}

result_t os_base::freemem(int64_t& retVal) {
	int freecount;
	size_t size = sizeof(freecount);

	if (sysctlbyname("vm.stats.vm.v_free_count", &freecount, &size, NULL, 0)
			== -1)
		return LastError();

	retVal = (int64_t) freecount * sysconf(_SC_PAGESIZE);

	return 0;
}

result_t os_base::CPUs(int32_t& retVal) {
	static int cpus = 0;

	if (cpus > 0) {
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

result_t os_base::CPUInfo(v8::Handle<v8::Array>& retVal) {
	retVal = v8::Array::New();

	v8::Local<v8::Object> cpuinfo;
	v8::Local<v8::Object> cputimes;

	unsigned int ticks = (unsigned int) sysconf(_SC_CLK_TCK), multiplier =
			((uint64_t) 1000L / ticks), cpuspeed, maxcpus, cur = 0;
	char model[512];
	long* cp_times;
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

	cp_times = (long int*) malloc(size);
	if (cp_times == NULL)
		return LastError();

	if (sysctlbyname("kern.cp_times", cp_times, &size, NULL, 0) < 0) {
		free(cp_times);
		return LastError();
	}

	for (i = 0; i < numcpus; i++) {
		cpuinfo = v8::Object::New();
		cputimes = v8::Object::New();
		cputimes->Set(
				v8::String::New("user"),
				v8::Number::New(
						(uint64_t)(cp_times[CP_USER + cur]) * multiplier));
		cputimes->Set(
				v8::String::New("nice"),
				v8::Number::New(
						(uint64_t)(cp_times[CP_NICE + cur]) * multiplier));
		cputimes->Set(
				v8::String::New("sys"),
				v8::Number::New((uint64_t)(cp_times[CP_SYS + cur]) * multiplier));
		cputimes->Set(
				v8::String::New("idle"),
				v8::Number::New(
						(uint64_t)(cp_times[CP_IDLE + cur]) * multiplier));
		cputimes->Set(
				v8::String::New("irq"),
				v8::Number::New(
						(uint64_t)(cp_times[CP_INTR + cur]) * multiplier));

		cpuinfo->Set(v8::String::New("model"), v8::String::New(model));
		cpuinfo->Set(v8::String::New("speed"), v8::Number::New(cpuspeed));

		cpuinfo->Set(v8::String::New("times"), cputimes);
		retVal->Set(i, cpuinfo);

		cur += CPUSTATES;
	}

	free(cp_times);

	return 0;
}

}

#endif
