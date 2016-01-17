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

static int32_t s_tls_rt;

void init_rt()
{
	s_tls_rt = exlib::Fiber::tlsAlloc();
}

void Runtime::reg()
{
	exlib::Fiber::tlsPut(s_tls_rt, this);
}

Runtime &Runtime::current()
{
	return *(Runtime *)exlib::Fiber::tlsGet(s_tls_rt);
}

} /* namespace fibjs */

