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
	virtual result_t get_mtime(JS_DATE& retVal);
	virtual result_t get_atime(JS_DATE& retVal);
	virtual result_t get_ctime(JS_DATE& retVal);

public:
	double size;
	JS_DATE mtime, atime, ctime;
};

}

#endif /* STAT_H_ */
