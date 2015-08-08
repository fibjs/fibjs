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
bool g_preemptive;

#ifdef x64
int32_t stack_size = 512;
#else
int32_t stack_size = 256;
#endif

void options(int32_t* argc, char *argv[])
{
	static char s_opts[64];
	static char s_sharmony[] = " --harmony --harmony_proxies"
	                           " --use_strict";

	v8::V8::SetFlagsFromString(s_sharmony, sizeof(s_sharmony) - 1);
	v8::V8::SetFlagsFromString(s_opts,
	                           sprintf(s_opts, "--stack_size=%d", stack_size - 16));

	int32_t df = 0;

	for (int32_t i = 0; i < *argc; i ++)
	{
		char* arg = argv[i];

		if (df)
			argv[i - df] = arg;

		if (!qstrcmp(arg, "--trace_fiber")) {
			df ++;
			Isolate::rt::g_trace = true;
		} else if (!qstrcmp(arg, "--perf")) {
			df ++;
			g_perf = true;
		} else if (!qstrcmp(arg, "--preemptive")) {
			df ++;
			g_preemptive = true;
		} else if (false) {
		}
	}

	if (df)
		*argc -= df;

	if (g_perf)
		Isolate::rt::g_trace = true;

#ifdef DEBUG
	Isolate::rt::g_trace = true;
#endif

	v8::V8::SetFlagsFromCommandLine(argc, argv, true);
}

}
