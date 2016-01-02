/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "Fiber.h"
#include "SandBox.h"

namespace fibjs
{

OSTls th_rt;
static int32_t s_tls_rt;

void init_rt()
{
	s_tls_rt = exlib::Fiber::tlsAlloc();
}

void Runtime::reg(void *rt)
{
	if (exlib::Service::hasService())
		exlib::Fiber::tlsPut(s_tls_rt, rt);
	else
		th_rt = rt;
}

Runtime &Runtime::now()
{
	if (exlib::Service::hasService())
		return *(Runtime *)exlib::Fiber::tlsGet(s_tls_rt);

	return *(Runtime *)th_rt;
}

} /* namespace fibjs */

