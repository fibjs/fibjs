/*
 * options.cpp
 *
 *  Created on: Jul 23, 2015
 *      Author: lion
 */

#include "console.h"
#include "Fiber.h"

namespace fibjs
{

bool g_perf;

#ifdef x64
int stack_size = 512;
#else
int stack_size = 256;
#endif

void options(int* argc, char *argv[])
{
	static char s_opts[64];
	static char s_sharmony[] = " --harmony --harmony_proxies"
	                           " --use_strict"
	                           " --gc_global";

	v8::V8::SetFlagsFromString(s_sharmony, sizeof(s_sharmony) - 1);
	v8::V8::SetFlagsFromString(s_opts,
	                           sprintf(s_opts, "--stack_size=%d", stack_size - 16));

	int df = 0;

	for (int i = 0; i < *argc; i ++)
	{
		char* arg = argv[i];

		if (df)
			argv[i - df] = arg;

		if (!fibjs::qstrcmp(arg, "--trace_fiber")) {
			df ++;
			fibjs::Isolate::rt::g_trace = true;
		} else if (!fibjs::qstrcmp(arg, "--perf")) {
			df ++;
			fibjs::g_perf = true;
		} else if (false) {

		}
	}

	if (df)
		*argc -= df;

	if (fibjs::g_perf)
		fibjs::Isolate::rt::g_trace = true;

#ifdef DEBUG
	fibjs::Isolate::rt::g_trace = true;
#endif

	v8::V8::SetFlagsFromCommandLine(argc, argv, true);
}

}
