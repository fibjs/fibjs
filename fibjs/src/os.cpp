/*
 * os.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include <osconfig.h>

#include "ifs/os.h"
#include "string.h"

#ifndef _WIN32
# include <unistd.h>  // gethostname, sysconf
# include <sys/utsname.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>
#else
#include <stdio.h>
#include <iphlpapi.h>

typedef struct
_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
static PROCNTQSI pNtQuerySystemInformation;

#define SystemBasicInformation 0 
#define SystemPerformanceInformation 2 
#define SystemTimeInformation 3
#define SystemProcessorPerformanceInformation 8

#define popen _popen
#define pclose _pclose

#endif

#ifdef MacOS
#include <mach/task.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <sys/sysctl.h>
#include <mach-o/dyld.h>
#else
#include <sys/sysinfo.h>
#include <dlfcn.h>
#endif

#include "utf8.h"
#include "Stat.h"
#include "List.h"
#include "File.h"
#include "BufferedStream.h"
#include "inetAddr.h"

namespace fibjs
{

result_t os_base::get_shell(std::string& retVal)
{
#ifdef _WIN32
	WCHAR szFileName[MAX_PATH];

	GetModuleFileNameW(NULL, szFileName, MAX_PATH);
	retVal = utf16to8String(szFileName);
#elif defined(Linux)
	size_t linksize = 256;
	char exeName[256] =
	{	0};

	if (readlink("/proc/self/exe", exeName, linksize) == -1)
		return LastError();

	retVal = exeName;
#elif defined(MacOS)
	char exeName[1024] = "";
	uint32_t size = sizeof(exeName);

	_NSGetExecutablePath(exeName, &size);

	retVal = exeName;
#endif
	return 0;
}

result_t os_base::get_hostname(std::string& retVal)
{
	char s[255];

	if (gethostname(s, 255) < 0)
		return LastError();

	retVal = s;
	return 0;
}

result_t os_base::get_type(std::string& retVal)
{

#ifdef _WIN32
	retVal = "Windows";
#else
	struct utsname info;

	uname(&info);
	retVal = info.sysname;
#endif

	return 0;
}

result_t os_base::get_version(std::string& retVal)
{
#ifdef _WIN32
	OSVERSIONINFO info =
	{	sizeof(info)};
	char release[256];

	if (GetVersionEx(&info) == 0)
		return LastError();

	sprintf(release, "%d.%d.%d", static_cast<int>(info.dwMajorVersion),
			static_cast<int>(info.dwMinorVersion), static_cast<int>(info.dwBuildNumber));
	retVal = release;
#else
	struct utsname info;

	uname(&info);
	retVal = info.release;
#endif

	return 0;
}

result_t os_base::get_arch(std::string& retVal)
{
#ifdef x64
	retVal = "x86_64";
#else
	retVal = "i686";
#endif

	return 0;
}

result_t os_base::uptime(double& retVal)
{
#ifdef _WIN32
	BYTE stack_buffer[4096];
	BYTE* malloced_buffer = NULL;
	BYTE* buffer = (BYTE*) stack_buffer;
	size_t buffer_size = sizeof(stack_buffer);
	DWORD data_size;

	PERF_DATA_BLOCK* data_block;
	PERF_OBJECT_TYPE* object_type;
	PERF_COUNTER_DEFINITION* counter_definition;

	DWORD i;

	for (;;)
	{
		LONG result;

		data_size = (DWORD) buffer_size;
		result = RegQueryValueExW(HKEY_PERFORMANCE_DATA, L"2", NULL, NULL,
				buffer, &data_size);
		if (result == ERROR_SUCCESS)
		{
			break;
		}
		else if (result != ERROR_MORE_DATA)
			return LastError();

		free(malloced_buffer);

		buffer_size *= 2;
		/* Don't let the buffer grow infinitely. */
		if (buffer_size > 1 << 20)
		{
			goto internalError;
		}

		buffer = malloced_buffer = (BYTE*) malloc(buffer_size);
		if (malloced_buffer == NULL)
			return LastError();
	}

	if (data_size < sizeof(*data_block))
		goto internalError;

	data_block = (PERF_DATA_BLOCK*) buffer;

	if (wmemcmp(data_block->Signature, L"PERF", 4) != 0)
		goto internalError;

	if (data_size < data_block->HeaderLength + sizeof(*object_type))
		goto internalError;

	object_type = (PERF_OBJECT_TYPE*) (buffer + data_block->HeaderLength);

	if (object_type->NumInstances != PERF_NO_INSTANCES)
		goto internalError;

	counter_definition = (PERF_COUNTER_DEFINITION*) (buffer
			+ data_block->HeaderLength + object_type->HeaderLength);
	for (i = 0; i < object_type->NumCounters; i++)
	{
		if ((BYTE*) counter_definition + sizeof(*counter_definition)
				> buffer + data_size)
		{
			break;
		}

		if (counter_definition->CounterNameTitleIndex == 674
				&& counter_definition->CounterSize == sizeof(uint64_t))
		{
			if (counter_definition->CounterOffset + sizeof(uint64_t) > data_size
					|| !(counter_definition->CounterType & PERF_OBJECT_TIMER))
			{
				goto internalError;
			}
			else
			{
				BYTE* address = (BYTE*) object_type
						+ object_type->DefinitionLength
						+ counter_definition->CounterOffset;
				uint64_t value = *((uint64_t*) address);
				retVal = (double) (object_type->PerfTime.QuadPart - value)
						/ (double) object_type->PerfFreq.QuadPart;
				free(malloced_buffer);
				return 0;
			}
		}

		counter_definition =
				(PERF_COUNTER_DEFINITION*) ((BYTE*) counter_definition
						+ counter_definition->ByteLength);
	}

	/* If we get here, the uptime value was not found. */
	free(malloced_buffer);
	return LastError();

	internalError: free(malloced_buffer);
	return LastError();
#elif defined(Linux)
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
#elif defined(MacOS)
	time_t now;
	struct timeval info;
	size_t size = sizeof(info);
	static int which[] =
	{ CTL_KERN, KERN_BOOTTIME };

	if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
		return LastError();

	now = ::time(NULL);

	retVal = (double) (now - info.tv_sec);
	return 0;
#endif
}

result_t os_base::loadavg(v8::Handle<v8::Array>& retVal)
{
	double avg[3] = {0, 0, 0};

#ifdef Linux
	  struct sysinfo info;

	  if (sysinfo(&info) < 0)
		  return LastError();

	  avg[0] = (double) info.loads[0] / 65536.0;
	  avg[1] = (double) info.loads[1] / 65536.0;
	  avg[2] = (double) info.loads[2] / 65536.0;
#elif defined(MacOS)
	struct loadavg info;
	size_t size = sizeof(info);
	int which[] =
	{ CTL_VM, VM_LOADAVG };

	if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
		return LastError();

	avg[0] = (double) info.ldavg[0] / info.fscale;
	avg[1] = (double) info.ldavg[1] / info.fscale;
	avg[2] = (double) info.ldavg[2] / info.fscale;
#endif

	retVal = v8::Array::New(3);
	retVal->Set(0, v8::Number::New(avg[0]));
	retVal->Set(1, v8::Number::New(avg[1]));
	retVal->Set(2, v8::Number::New(avg[2]));

	return 0;
}

result_t os_base::totalmem(int64_t& retVal)
{
#ifdef _WIN32
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);

	if(!GlobalMemoryStatusEx(&memory_status))
		return LastError();

	retVal = memory_status.ullTotalPhys;
	return 0;
#elif defined(Linux)
	retVal = sysconf(_SC_PAGESIZE) * sysconf(_SC_PHYS_PAGES);
	return 0;
#elif defined(MacOS)
	uint64_t info;
	int which[] =
	{ CTL_HW, HW_MEMSIZE };
	size_t size = sizeof(info);

	if (sysctl(which, 2, &info, &size, NULL, 0) < 0)
		return LastError();

	retVal = info;

	return 0;
#endif
}

result_t os_base::freemem(int64_t& retVal)
{
#ifdef _WIN32
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);

	if(!GlobalMemoryStatusEx(&memory_status))
		return LastError();

	retVal = memory_status.ullAvailPhys;
	return 0;
#elif defined(Linux)
	retVal = sysconf(_SC_PAGESIZE) * sysconf(_SC_AVPHYS_PAGES);
	return 0;
#elif defined(MacOS)
	vm_statistics_data_t info;
	mach_msg_type_number_t count = sizeof(info) / sizeof(integer_t);

	if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t) &info,
			&count) != KERN_SUCCESS)
		return LastError();

	retVal = info.free_count * sysconf(_SC_PAGESIZE);
	return 0;
#endif
}

result_t os_base::CPUs(int32_t& retVal)
{
	static int cpus = 0;

	if (cpus > 0)
	{
		retVal = cpus;
		return 0;
	}

#ifdef _WIN32
	SYSTEM_INFO system_info;

	GetSystemInfo(&system_info);
	cpus = system_info.dwNumberOfProcessors;

	retVal = cpus;
#elif defined(Linux)
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
#elif defined(MacOS)
	natural_t numcpus;
	mach_msg_type_number_t count;
	processor_cpu_load_info_data_t *info;
	if (host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numcpus,
			reinterpret_cast<processor_info_array_t*>(&info),
			&count) != KERN_SUCCESS)
		return LastError();

	vm_deallocate(mach_task_self(), (vm_address_t) info, count);

	retVal = cpus = numcpus;
#endif

	return 0;
}

result_t os_base::CPUInfo(v8::Handle<v8::Array>& retVal)
{
	retVal = v8::Array::New();

#ifdef _WIN32

	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* sppi;
	DWORD sppi_size;
	SYSTEM_INFO system_info;
	DWORD cpu_count, i, r;
	ULONG result_size;
	result_t hr = 0;

	if (pNtQuerySystemInformation == NULL)
		pNtQuerySystemInformation = (PROCNTQSI) GetProcAddress(
				GetModuleHandle("ntdll"), "NtQuerySystemInformation");

	if (pNtQuerySystemInformation == NULL)
		return LastError();

	GetSystemInfo(&system_info);
	cpu_count = system_info.dwNumberOfProcessors;

	sppi_size = sizeof(*sppi) * cpu_count;
	sppi = (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*) malloc(sppi_size);
	if (!sppi)
		return LastError();

	r = pNtQuerySystemInformation(SystemProcessorPerformanceInformation, sppi,
			sppi_size, &result_size);
	if (r != ERROR_SUCCESS || result_size != sppi_size)
	{
		hr = LastError();
		free(sppi);
		return hr;
	}

	for (i = 0; i < cpu_count; i++)
	{
		WCHAR key_name[128];
		HKEY processor_key;
		DWORD cpu_speed;
		DWORD cpu_speed_size = sizeof(cpu_speed);
		char cpu_brand[256];
		DWORD cpu_brand_size = sizeof(cpu_brand);

		_snwprintf(key_name, sizeof(key_name) / sizeof(key_name[0]),
				L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);

		r = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_name, 0, KEY_QUERY_VALUE,
				&processor_key);
		if (r != ERROR_SUCCESS)
		{
			hr = LastError();
			free(sppi);
			return hr;
		}

		if (RegQueryValueExW(processor_key, L"~MHz", NULL, NULL,
				(BYTE*) &cpu_speed, &cpu_speed_size) != ERROR_SUCCESS)
		{
			hr = LastError();
			RegCloseKey(processor_key);
			free(sppi);
			return hr;
		}

		if (RegQueryValueExA(processor_key, "ProcessorNameString", NULL, NULL,
				(BYTE*) &cpu_brand, &cpu_brand_size) != ERROR_SUCCESS)
		{
			hr = LastError();
			RegCloseKey(processor_key);
			free(sppi);
			return hr;
		}

		RegCloseKey(processor_key);

		v8::Local<v8::Object> times_info = v8::Object::New();
		times_info->Set(v8::String::New("user"),
				v8::Integer::New(
						(int32_t) (sppi[i].UserTime.QuadPart / 10000)));
		times_info->Set(v8::String::New("nice"), v8::Integer::New(0));
		times_info->Set(v8::String::New("sys"),
				v8::Integer::New(
						(int32_t) (sppi[i].KernelTime.QuadPart
								- sppi[i].IdleTime.QuadPart) / 10000));
		times_info->Set(v8::String::New("idle"),
				v8::Integer::New(
						(int32_t) (sppi[i].IdleTime.QuadPart / 10000)));
		times_info->Set(v8::String::New("irq"),
				v8::Integer::New(
						(int32_t) (sppi[i].InterruptTime.QuadPart / 10000)));

		v8::Local<v8::Object> cpu_info = v8::Object::New();
		cpu_info->Set(v8::String::New("model"), v8::String::New(cpu_brand));
		cpu_info->Set(v8::String::New("speed"), v8::Integer::New(cpu_speed));
		cpu_info->Set(v8::String::New("times"), times_info);
		retVal->Set(i, cpu_info);
	}

	return hr;
#elif defined(Linux)
	v8::Local<v8::Object> cpuinfo;
	v8::Local<v8::Object> cputimes;
	unsigned int ticks = (unsigned int)sysconf(_SC_CLK_TCK),
	multiplier = ((uint64_t)1000L / ticks), cpuspeed;
	int numcpus = 0, i = 0;
	unsigned long long ticks_user, ticks_sys, ticks_idle, ticks_nice, ticks_intr;
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
					model[strlen(model)-1] = 0;
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

			sscanf(line, "%*s %llu %llu %llu %llu %*llu %llu",
					&ticks_user, &ticks_nice, &ticks_sys, &ticks_idle, &ticks_intr);
			snprintf(speedPath, sizeof(speedPath),
					"/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_max_freq", i);

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
			cputimes->Set(v8::String::New("user"), v8::Number::New(ticks_user * multiplier));
			cputimes->Set(v8::String::New("nice"), v8::Number::New(ticks_nice * multiplier));
			cputimes->Set(v8::String::New("sys"), v8::Number::New(ticks_sys * multiplier));
			cputimes->Set(v8::String::New("idle"), v8::Number::New(ticks_idle * multiplier));
			cputimes->Set(v8::String::New("irq"), v8::Number::New(ticks_intr * multiplier));

			cpuinfo->Set(v8::String::New("model"), v8::String::New(model));
			cpuinfo->Set(v8::String::New("speed"), v8::Number::New(cpuspeed));

			cpuinfo->Set(v8::String::New("times"), cputimes);
			retVal->Set(i++, cpuinfo);
		}
		fclose(fpStat);
	}
#elif defined(MacOS)
	v8::Local<v8::Object> cpuinfo;
	v8::Local<v8::Object> cputimes;
	unsigned int ticks = (unsigned int) sysconf(_SC_CLK_TCK), multiplier =
			((uint64_t) 1000L / ticks);
	char model[512];
	uint64_t cpuspeed;
	size_t size;

	size = sizeof(model);
	if (sysctlbyname("hw.model", &model, &size, NULL, 0) < 0)
		return LastError();

	size = sizeof(cpuspeed);
	if (sysctlbyname("hw.cpufrequency", &cpuspeed, &size, NULL, 0) < 0)
		return LastError();

	natural_t numcpus;
	mach_msg_type_number_t count;
	processor_cpu_load_info_data_t *info;
	if (host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numcpus,
			reinterpret_cast<processor_info_array_t*>(&info),
			&count) != KERN_SUCCESS)
		return LastError();

	retVal = v8::Array::New(numcpus);
	for (unsigned int i = 0; i < numcpus; i++)
	{
		cpuinfo = v8::Object::New();
		cputimes = v8::Object::New();
		cputimes->Set(v8::String::New("user"),
				v8::Number::New(
						(uint64_t) (info[i].cpu_ticks[0]) * multiplier));
		cputimes->Set(v8::String::New("nice"),
				v8::Number::New(
						(uint64_t) (info[i].cpu_ticks[3]) * multiplier));
		cputimes->Set(v8::String::New("sys"),
				v8::Number::New(
						(uint64_t) (info[i].cpu_ticks[1]) * multiplier));
		cputimes->Set(v8::String::New("idle"),
				v8::Number::New(
						(uint64_t) (info[i].cpu_ticks[2]) * multiplier));
		cputimes->Set(v8::String::New("irq"), v8::Number::New(0));

		cpuinfo->Set(v8::String::New("model"), v8::String::New(model));
		cpuinfo->Set(v8::String::New("speed"),
				v8::Number::New(cpuspeed / 1000000));

		cpuinfo->Set(v8::String::New("times"), cputimes);
		retVal->Set(i, cpuinfo);
	}
	vm_deallocate(mach_task_self(), (vm_address_t) info, count);

#endif

	return 0;
}

result_t os_base::networkInfo(v8::Handle<v8::Object>& retVal)
{
#ifdef _WIN32
	unsigned long size = 0;
	IP_ADAPTER_ADDRESSES* adapter_addresses;
	IP_ADAPTER_ADDRESSES* adapter_address;
	IP_ADAPTER_UNICAST_ADDRESS_XP* unicast_address;

	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size)
			!= ERROR_BUFFER_OVERFLOW)
		return LastError();

	adapter_addresses = (IP_ADAPTER_ADDRESSES*) malloc(size);
	if (!adapter_addresses)
		return LastError();

	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapter_addresses, &size)
			!= ERROR_SUCCESS)
		return LastError();

	retVal = v8::Object::New();

	for (adapter_address = adapter_addresses; adapter_address != NULL;
			adapter_address = adapter_address->Next)
	{
		v8::Local<v8::Array> ret;
		v8::Local<v8::Object> o;
		v8::Local<v8::String> name, ipaddr, family;

		if (adapter_address->OperStatus != IfOperStatusUp)
			continue;

		name = v8::String::New((const uint16_t*) adapter_address->FriendlyName);

		ret = v8::Array::New();
		retVal->Set(name, ret);

		unicast_address =
				(IP_ADAPTER_UNICAST_ADDRESS_XP*) adapter_address->FirstUnicastAddress;
		while (unicast_address)
		{
			inetAddr* sock_addr = (inetAddr*) unicast_address->Address.lpSockaddr;

			o = v8::Object::New();
			o->Set(v8::String::New("address"), v8::String::New(sock_addr->str().c_str()));
			o->Set(v8::String::New("family"), sock_addr->family() == net_base::_AF_INET6 ?
					v8::String::New("IPv6") : v8::String::New("IPv4"));
			o->Set(v8::String::New("internal"),
					adapter_address->IfType == IF_TYPE_SOFTWARE_LOOPBACK ?
							v8::True() : v8::False());

			ret->Set(ret->Length(), o);

			unicast_address = unicast_address->Next;
		}
	}

	free(adapter_addresses);

	return 0;
#else
	struct ::ifaddrs *addrs, *ent;
	struct ::sockaddr_in *in4;
	struct ::sockaddr_in6 *in6;
	char ip[INET6_ADDRSTRLEN];

	if (getifaddrs(&addrs) != 0)
		return LastError();

	retVal = v8::Object::New();

	for (ent = addrs; ent != NULL; ent = ent->ifa_next)
	{
		v8::Local<v8::Array> ret;
		v8::Local<v8::Object> o;
		v8::Local<v8::String> name, ipaddr, family;

		bzero(&ip, sizeof(ip));
		if (!(ent->ifa_flags & IFF_UP && ent->ifa_flags & IFF_RUNNING))
			continue;

		if (ent->ifa_addr == NULL)
			continue;

		if (ent->ifa_addr->sa_family != AF_INET6
				&& ent->ifa_addr->sa_family != AF_INET)
			continue;

		name = v8::String::New(ent->ifa_name);
		if (retVal->Has(name))
		{
			ret = v8::Local<v8::Array>::Cast(retVal->Get(name));
		}
		else
		{
			ret = v8::Array::New();
			retVal->Set(name, ret);
		}

		if (ent->ifa_addr->sa_family == AF_INET6)
		{
			in6 = (struct sockaddr_in6 *) ent->ifa_addr;
			inet_ntop(AF_INET6, &(in6->sin6_addr), ip, INET6_ADDRSTRLEN);
			family = v8::String::New("IPv6");
		}
		else if (ent->ifa_addr->sa_family == AF_INET)
		{
			in4 = (struct sockaddr_in *) ent->ifa_addr;
			inet_ntop(AF_INET, &(in4->sin_addr), ip, INET6_ADDRSTRLEN);
			family = v8::String::New("IPv4");
		}

		o = v8::Object::New();
		o->Set(v8::String::New("address"), v8::String::New(ip));
		o->Set(v8::String::New("family"), family);
		o->Set(v8::String::New("internal"),
				ent->ifa_flags & IFF_LOOPBACK ? v8::True() : v8::False());

		ret->Set(ret->Length(), o);
	}

	freeifaddrs(addrs);
#endif

	return 0;
}

result_t os_base::time(const char* tmString, date_t& retVal)
{
	if (!tmString || !*tmString)
		retVal.now();
	else
		retVal.parse(tmString);

	return 0;
}

result_t os_base::get_timezone(int32_t& retVal)
{
	retVal = date_t::LocalOffset();
	return 0;
}

result_t os_base::stat(const char* path, obj_ptr<Stat_base>& retVal,
		exlib::AsyncEvent* ac)
{
	obj_ptr<Stat> pStat = new Stat();

	result_t hr = pStat->getStat(path);
	if (hr < 0)
		return hr;

	retVal = pStat;

	return 0;
}

#ifndef _WIN32

result_t os_base::exists(const char* path, bool& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	retVal = access(path, F_OK) == 0;
	return 0;
}

result_t os_base::unlink(const char* path, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	if (::unlink(path))
		return LastError();

	return 0;
}

result_t os_base::mkdir(const char* path, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	if (::mkdir(path, 715))
		return LastError();

	return 0;
}

result_t os_base::rmdir(const char* path, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	if (::rmdir(path))
		return LastError();

	return 0;
}

result_t os_base::rename(const char* from, const char* to,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	if (::rename(from, to))
		return LastError();

	return 0;
}

result_t os_base::readdir(const char* path, obj_ptr<List_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	DIR * dp;
	struct dirent * ep;
	std::string fpath;
	result_t hr;
	obj_ptr<List> oa;

	dp = ::opendir(path);
	if (dp == NULL)
		return LastError();

	oa = new List();

	while ((ep = ::readdir(dp)))
	{
		obj_ptr<Stat_base> fstat;

		fpath = path;
		fpath += '/';
		fpath += ep->d_name;

		hr = stat(fpath.c_str(), fstat, NULL);
		if (hr >= 0)
			oa->append(fstat);
	}
	::closedir(dp);

	retVal = oa;

	return 0;
}

#else

result_t os_base::exists(const char* path, bool& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
	return CALL_E_NOSYNC;

	retVal = _waccess(UTF8_W(path), 0) == 0;
	return 0;
}

result_t os_base::unlink(const char* path, exlib::AsyncEvent* ac)
{
	if (!ac)
	return CALL_E_NOSYNC;

	if(::_wunlink(UTF8_W(path)))
	return LastError();

	return 0;
}

result_t os_base::mkdir(const char* path, exlib::AsyncEvent* ac)
{
	if (!ac)
	return CALL_E_NOSYNC;

	if (::_wmkdir(UTF8_W(path)))
	return LastError();

	return 0;
}

result_t os_base::rmdir(const char* path, exlib::AsyncEvent* ac)
{
	if (!ac)
	return CALL_E_NOSYNC;

	if (::_wrmdir(UTF8_W(path)))
	return LastError();

	return 0;
}

result_t os_base::rename(const char* from, const char* to, exlib::AsyncEvent* ac)
{
	if (!ac)
	return CALL_E_NOSYNC;

	if (::_wrename(UTF8_W(from), UTF8_W(to)))
	return LastError();

	return 0;
}

result_t os_base::readdir(const char* path, obj_ptr<List_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
	return CALL_E_NOSYNC;

	WIN32_FIND_DATAW fd;
	HANDLE hFind;
	std::wstring fpath;
	obj_ptr<List> oa;

	fpath = utf8to16String(path);
	fpath.append(L"/*", 2);

	hFind = FindFirstFileW(fpath.c_str(), &fd);
	if (hFind == INVALID_HANDLE_VALUE)
	return LastError();

	oa = new List();

	do
	{
		obj_ptr<Stat> pStat = new Stat();
		pStat->fill(fd);
		oa->append(pStat);
	}while(FindNextFileW(hFind, &fd));

	FindClose(hFind);

	retVal = oa;

	return 0;
}

#endif

void clearModule();
void flushLog();

result_t os_base::exit(int32_t code)
{
	flushLog();
	clearModule();

	::exit(code);
	return 0;
}

result_t os_base::system(const char* cmd, int32_t& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

#ifdef _WIN32
	retVal = ::system(cmd);
#else
	retVal = ::system(cmd) >> 8;
#endif

	return 0;
}

result_t os_base::exec(const char* cmd, obj_ptr<BufferedStream_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	FILE* pPipe = popen(cmd, "r");
	if (pPipe == NULL)
		return LastError();

	retVal = new BufferedStream(new File(pPipe, true));
	retVal->set_EOL("\n");

	return 0;
}

}

