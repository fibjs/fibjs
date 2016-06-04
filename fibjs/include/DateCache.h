/*
 * DataCache.h
 *
 *  Created on: Jul 27, 2013
 *      Author: lion
 */

#include "utils.h"

#ifndef DATACACHE_H_
#define DATACACHE_H_

namespace fibjs
{

class DateCache
{
public:
	DateCache();
	~DateCache();

public:
	int64_t ToLocal(int64_t time_ms);
	int64_t ToUTC(int64_t time_ms);
	int32_t LocalOffset();

private:
	void *m_pdc;
};

}

#endif
