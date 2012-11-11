/*
 * os_linux.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <osconfig.h>

#ifdef Linux

#include "ifs/os.h"
#include "string.h"

# include <unistd.h>  // gethostname, sysconf
# include <sys/utsname.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>

#include <sys/sysinfo.h>
#include <dlfcn.h>

#ifndef CLOCK_BOOTTIME
#define CLOCK_BOOTTIME 7
#endif

#include "inetAddr.h"

namespace fibjs
{

result_t os_base::uptime(double& retVal)
{
	  static volatile int no_clock_boottime;
	  struct timespec now;
	  int r;
	  int (*fngettime)(clockid_t, struct timespec *);

	  void *handle = dlopen ("librt.so", RTLD_LAZY);
	  if (!handle)
		  return LastError();

	  fngettime = (int (*)(clockid_t, struct timespec *))dlsym(handle, "clock_gettime");
	  if(!fngettime)
	  {
		  dlclose(handle);
		  return LastError();
	  }

	  if (no_clock_boottime) {
	    retry: r = fngettime(CLOCK_MONOTONIC, &now);
	  }
	  else if ((r = fngettime(CLOCK_BOOTTIME, &now)) && errno == EINVAL) {
	    no_clock_boottime = 1;
	    goto retry;
	  }

	  dlclose(handle);

	  if (r)
		  return LastError();

	  retVal = now.tv_sec;
	  retVal += (double)now.tv_nsec / 1000000000.0;

	  return 0;
}

result_t os_base::loadavg(v8::Handle<v8::Array>& retVal)
{
	double avg[3] = {0, 0, 0};

	struct sysinfo info;

	if (sysinfo(&info) < 0)
		return LastError();

	avg[0] = (double) info.loads[0] / 65536.0;
	avg[1] = (double) info.loads[1] / 65536.0;
	avg[2] = (double) info.loads[2] / 65536.0;

	retVal = v8::Array::New(3);
	retVal->Set(0, v8::Number::New(avg[0]));
	retVal->Set(1, v8::Number::New(avg[1]));
	retVal->Set(2, v8::Number::New(avg[2]));

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

result_t os_base::CPUs(int32_t& retVal)
{
	static int cpus = 0;

	if (cpus > 0)
	{
		retVal = cpus;
		return 0;
	}

	int numcpus = 0;
	char line[512];
	FILE *fpModel = fopen("/proc/cpuinfo", "r");

	if (fpModel)
	{
		while (fgets(line, 511, fpModel) != NULL)
			if (strncmp(line, "model name", 10) == 0)
				numcpus++;
		fclose(fpModel);

		retVal = cpus = numcpus;
	}
	else
		return LastError();

	return 0;
}

result_t os_base::CPUInfo(v8::Handle<v8::Array>& retVal)
{
	retVal = v8::Array::New();

	v8::Local<v8::Object> cpuinfo;
	v8::Local<v8::Object> cputimes;
	unsigned int ticks = (unsigned int) sysconf(_SC_CLK_TCK), multiplier =
			((uint64_t) 1000L / ticks), cpuspeed;
	int numcpus = 0, i = 0;
	unsigned long long ticks_user, ticks_sys, ticks_idle, ticks_nice,
			ticks_intr;
	char line[512], speedPath[256], model[512];
	FILE *fpStat = fopen("/proc/stat", "r");
	FILE *fpModel = fopen("/proc/cpuinfo", "r");
	FILE *fpSpeed;

	if (fpModel)
	{
		while (fgets(line, 511, fpModel) != NULL)
		{
			if (strncmp(line, "model name", 10) == 0)
			{
				numcpus++;
				if (numcpus == 1)
				{
					char *p = strchr(line, ':') + 2;
					strcpy(model, p);
					model[strlen(model) - 1] = 0;
				}
			}
			else if (strncmp(line, "cpu MHz", 7) == 0)
			{
				if (numcpus == 1)
					sscanf(line, "%*s %*s : %u", &cpuspeed);
			}
		}
		fclose(fpModel);
	}

	if (fpStat)
	{
		while (fgets(line, 511, fpStat) != NULL)
		{
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

			if (fpSpeed)
			{
				if (fgets(line, 511, fpSpeed) != NULL)
				{
					sscanf(line, "%u", &cpuspeed);
					cpuspeed /= 1000;
				}
				fclose(fpSpeed);
			}

			cpuinfo = v8::Object::New();
			cputimes = v8::Object::New();
			cputimes->Set(v8::String::New("user"),
					v8::Number::New(ticks_user * multiplier));
			cputimes->Set(v8::String::New("nice"),
					v8::Number::New(ticks_nice * multiplier));
			cputimes->Set(v8::String::New("sys"),
					v8::Number::New(ticks_sys * multiplier));
			cputimes->Set(v8::String::New("idle"),
					v8::Number::New(ticks_idle * multiplier));
			cputimes->Set(v8::String::New("irq"),
					v8::Number::New(ticks_intr * multiplier));

			cpuinfo->Set(v8::String::New("model"), v8::String::New(model));
			cpuinfo->Set(v8::String::New("speed"), v8::Number::New(cpuspeed));

			cpuinfo->Set(v8::String::New("times"), cputimes);
			retVal->Set(i++, cpuinfo);
		}
		fclose(fpStat);
	}

	return 0;
}

}

#endif

