/*
 * hyperscan.cpp
 *
 *  Created on: Sep 8, 2017
 *      Author: asionius
 */

#include "ifs/hyperscan.h"
#include "HsRegExp.h"

namespace fibjs {

DECLARE_MODULE(hyperscan);

#ifndef _WIN32

result_t hyperscan_base::compile(exlib::string pattern, exlib::string flag, obj_ptr<HsRegExp_base>& retVal)
{
	result_t hr;
	obj_ptr<HsRegExp> obj = new HsRegExp();

	hr = obj->compile(pattern, flag);
	if (hr < 0)
		return CHECK_ERROR(hr);

	retVal = obj;
	return 0;
}

result_t hyperscan_base::compile(v8::Local<v8::Array> patterns, v8::Local<v8::Array> flags, obj_ptr<HsRegExp_base>& retVal)
{
	result_t hr;
	obj_ptr<HsRegExp> obj = new HsRegExp();

	hr = obj->compile(patterns, flags);
	if (hr < 0)
		return CHECK_ERROR(hr);

	retVal = obj;
	return 0;
}

#else

result_t hyperscan_base::compile(exlib::string pattern, exlib::string flag, obj_ptr<HsRegExp_base>& retVal)
{
	return 0;
}

result_t hyperscan_base::compile(v8::Local<v8::Array> patterns, v8::Local<v8::Array> flags, obj_ptr<HsRegExp_base>& retVal)
{
	return 0;
}

#endif

}
