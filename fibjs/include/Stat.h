/*
 * Stat.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef STAT_H_
#define STAT_H_

#include "ifs/Stat.h"

namespace fibjs
{

class Stat : public Stat_base
{
public:
	// Stat_base
	virtual result_t get_size(double& retVal);
	virtual result_t get_mtime(int64_t& retVal);
	virtual result_t get_atime(int64_t& retVal);
	virtual result_t get_ctime(int64_t& retVal);

public:
	double size;
	int64_t mtime, atime, ctime;
};

}

#endif /* STAT_H_ */
