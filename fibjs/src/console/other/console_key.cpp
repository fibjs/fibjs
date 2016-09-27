/*
 * console_key.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#ifndef _WIN32

namespace fibjs
{

result_t console_base::keyDown(exlib::string key, exlib::string modifier)
{
	return 0;
}

result_t console_base::keyDown(exlib::string key, v8::Local<v8::Array> modifier)
{
	return 0;
}

result_t console_base::keyUp(exlib::string key, exlib::string modifier)
{
	return 0;
}

result_t console_base::keyUp(exlib::string key, v8::Local<v8::Array> modifier)
{
	return 0;
}

result_t console_base::keyTap(exlib::string key, exlib::string modifier)
{
	return 0;
}

result_t console_base::keyTap(exlib::string key, v8::Local<v8::Array> modifier)
{
	return 0;
}

result_t console_base::typeString(exlib::string text)
{
	return 0;
}

}

#endif
