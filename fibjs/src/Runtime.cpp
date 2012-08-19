/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "Fiber.h"

#ifdef MacOS
#include <sys/sysctl.h>
#endif

namespace fibjs
{

#ifdef MacOS

static pthread_key_t keyRuntime;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static intptr_t kMacTlsBaseOffset = -1;

intptr_t MacTlsBaseOffset()
{
	if (kMacTlsBaseOffset == -1)
	{
		const size_t kBufferSize = 128;
		char buffer[kBufferSize];
		size_t buffer_size = kBufferSize;
		int ctl_name[] =
		{ CTL_KERN, KERN_OSRELEASE };
		sysctl(ctl_name, 2, buffer, &buffer_size, NULL, 0);
		buffer[kBufferSize - 1] = '\0';
		char* period_pos = strchr(buffer, '.');
		*period_pos = '\0';
		int kernel_version_major = static_cast<int>(strtol(buffer, NULL, 10));

		if (kernel_version_major < 11)
		{
#if defined(I386)
			kMacTlsBaseOffset = 0x48;
#else
			kMacTlsBaseOffset = 0x60;
#endif
		}
		else
		{
			kMacTlsBaseOffset = 0;
		}
	}

	return kMacTlsBaseOffset;
}

inline intptr_t FastThreadLocal(intptr_t index)
{
	intptr_t result;
#if defined(I386)
	asm("movl %%gs:(%1,%2,4), %0;"
			:"=r"(result)
			:"r"(kMacTlsBaseOffset), "r"(index));
#else
	asm("movq %%gs:(%1,%2,8), %0;"
			:"=r"(result)
			:"r"(kMacTlsBaseOffset), "r"(index));
#endif
	return result;
}

static void once_run(void)
{
	MacTlsBaseOffset();
	pthread_key_create(&keyRuntime, NULL);
}

void Runtime::reg(Runtime* rt)
{
	pthread_once(&once, once_run);
	pthread_setspecific(keyRuntime, rt);
}

#else

#if defined(_MSC_VER)
__declspec(thread) Runtime* th_rt = NULL;
#else
__thread Runtime* th_rt = NULL;
#endif

void Runtime::reg(Runtime* rt)
{
	th_rt = rt;
}

#endif

Runtime& Runtime::now()
{
	exlib::Service* pService = exlib::Service::getFiberService();

	if (pService)
	{
		JSFiber* fb = (JSFiber*) pService->tlsGet(g_tlsCurrent);

		if (!fb)
		{
			fb = new JSFiber();
			pService->tlsPut(g_tlsCurrent, fb);
			fb->Ref();
		}

		return fb->runtime();
	}

#ifdef MacOS
	return *(Runtime*) FastThreadLocal(keyRuntime);
#else
	return *th_rt;
#endif
}

} /* namespace fibjs */
