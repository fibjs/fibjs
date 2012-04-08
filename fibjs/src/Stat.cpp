/*
 * Stat.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "Stat.h"

namespace fibjs
{

result_t Stat::get_size(double& retVal)
{
	retVal = size;
	return 0;
}

result_t Stat::get_mtime(JS_DATE& retVal)
{
	retVal = mtime;
	return 0;
}

result_t Stat::get_atime(JS_DATE& retVal)
{
	retVal = atime;
	return 0;
}

result_t Stat::get_ctime(JS_DATE& retVal)
{
	retVal = ctime;
	return 0;
}

}
