/*
 * RadosStat.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: asionius
 */

#include "object.h"
#include "RadosStat.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {
#ifndef _WIN32
result_t RadosStat::get_size(int64_t& retVal)
{
	retVal = m_size;
	return 0;
}
result_t RadosStat::get_mtime(date_t& retVal)
{
	retVal = m_mtime;
	return 0;
}
#else
result_t RadosStat::get_size(int64_t& retVal)
{}
result_t RadosStat::get_mtime(date_t& retVal)
{}
result_t RadosStat::get_stat()
{}

#endif

}
