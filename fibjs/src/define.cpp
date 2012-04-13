/*
 * define.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/global.h"

namespace fibjs
{

v8::Handle<v8::Value> _define(const v8::Arguments& args)
{
	return v8::Undefined();
}

result_t global_base::define(const char* id, v8::Handle<v8::Array> deps, v8::Handle<v8::Value> factory)
{
	return 0;
}

}
