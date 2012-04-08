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

result_t Stat::get_mtime(int64_t& retVal)
{
	retVal = mtime;
	return 0;
}

result_t Stat::get_atime(int64_t& retVal)
{
	retVal = atime;
	return 0;
}

result_t Stat::get_ctime(int64_t& retVal)
{
	retVal = ctime;
	return 0;
}

}
