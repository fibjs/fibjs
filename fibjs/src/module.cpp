/*
 * require.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "ifs/global.h"
#include "ifs/vm.h"

namespace fibjs
{

result_t global_base::run(const char* fname)
{
	obj_ptr < SandBox_base > sbox;
	result_t hr = vm_base::current(sbox);
	if (hr < 0)
		return hr;

	return sbox->run(fname);
}

result_t global_base::require(const char* id, v8::Handle<v8::Value>& retVal)
{
	obj_ptr < SandBox_base > sbox;
	result_t hr = vm_base::current(sbox);
	if (hr < 0)
		return hr;

	return sbox->require(id, retVal);
}

}
