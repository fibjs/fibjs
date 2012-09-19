/*
 * process.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/process.h"
#include "File.h"
#include "BufferedStream.h"

#ifndef _WIN32
#ifdef MacOS
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h>
#else
#include <sys/param.h>
#include <sys/sysinfo.h>
#include <dlfcn.h>
#endif
#else
#include <psapi.h>
#include "utf8.h"

#define pclose _pclose
#endif

namespace fibjs
{

static obj_ptr<BufferedStream_base> s_err, s_in, s_out;

result_t process_base::get_stderr(obj_ptr<BufferedStream_base>& retVal)
{
	if (s_err == NULL)
	{
		s_err = new BufferedStream(new File(stderr, true));
		s_err->set_EOL("\n");
	}

	retVal = s_err;
	return 0;
}

result_t process_base::get_stdin(obj_ptr<BufferedStream_base>& retVal)
{
	if (s_in == NULL)
	{
		s_in = new BufferedStream(new File(stdin, true));
		s_in->set_EOL("\n");
	}

	retVal = s_in;
	return 0;
}

result_t process_base::get_stdout(obj_ptr<BufferedStream_base>& retVal)
{
	if (s_out == NULL)
	{
		s_out = new BufferedStream(new File(stdout, true));
		s_out->set_EOL("\n");
	}

	retVal = s_out;
	return 0;
}

static int s_argc;
static char** s_argv;

void init_argv(int argc, char** argv)
{
	s_argc = argc;
	s_argv = argv;
}

result_t process_base::get_argv(v8::Handle<v8::Array>& retVal)
{
	v8::Handle<v8::Array> args = v8::Array::New(s_argc);

	for(int i = 0; i < s_argc; i ++)
		args->Set(i, v8::String::New(s_argv[i]));

	retVal = args;

	return 0;
}

result_t process_base::get_execPath(std::string& retVal)
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

void clearModule();
void flushLog();

result_t process_base::exit(int32_t code)
{
	flushLog();
	clearModule();

	::exit(code);
	return 0;
}

result_t process_base::memoryUsage(v8::Handle<v8::Object>& retVal)
{
	size_t rss = 0;

#ifdef _WIN32
	HANDLE current_process;
	PROCESS_MEMORY_COUNTERS pmc;

	current_process = GetCurrentProcess();

	if (!GetProcessMemoryInfo(current_process, &pmc, sizeof(pmc)))
		return LastError();

	rss = pmc.WorkingSetSize;

#elif defined(Linux)
	FILE* f;
	int itmp;
	char ctmp;
	unsigned int utmp;
	size_t page_size = getpagesize();
	char *cbuf;
	int foundExeEnd;
	static char buf[MAXPATHLEN + 1];

	f = fopen("/proc/self/stat", "r");
	if (!f)
		return LastError();

	/* PID */
	if (fscanf(f, "%d ", &itmp) == 0)
		goto error;
	/* coverity[secure_coding] */
	/* Exec file */
	cbuf = buf;
	foundExeEnd = 0;
	if (fscanf(f, "%c", cbuf++) == 0)
		goto error;

	while (1)
	{
		if (fscanf(f, "%c", cbuf) == 0)
			goto error;
		if (*cbuf == ')')
		{
			foundExeEnd = 1;
		}
		else if (foundExeEnd && *cbuf == ' ')
		{
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
	rss = (size_t) utmp * page_size;

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

	error: fclose(f);
#elif defined(MacOS)
	struct task_basic_info t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

	if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t) &t_info,
			&t_info_count) != KERN_SUCCESS)
		return LastError();

	rss = t_info.resident_size;
#endif

	v8::Handle<v8::Object> info = v8::Object::New();

	v8::HeapStatistics v8_heap_stats;
	v8::V8::GetHeapStatistics(&v8_heap_stats);
	info->Set(v8::String::New("rss"), v8::Integer::New((int32_t)rss));
	info->Set(v8::String::New("heapTotal"),
			v8::Integer::New((int32_t)v8_heap_stats.total_heap_size()));
	info->Set(v8::String::New("heapUsed"),
			v8::Integer::New((int32_t)v8_heap_stats.used_heap_size()));

	retVal = info;

	return 0;
}

result_t process_base::system(const char* cmd, int32_t& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

#ifdef _WIN32
	retVal = ::_wsystem(UTF8_W(cmd));
#else
	retVal = ::system(cmd) >> 8;
#endif

	return 0;
}

result_t process_base::exec(const char* cmd,
		obj_ptr<BufferedStream_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

#ifdef _WIN32
	FILE* pPipe = _wpopen(UTF8_W(cmd), L"r");
#else
	FILE* pPipe = popen(cmd, "r");
#endif

	if (pPipe == NULL)
		return LastError();

	retVal = new BufferedStream(new File(pPipe, true));
	retVal->set_EOL("\n");

	return 0;
}

}

