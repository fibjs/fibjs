/*
 * options.h
 *
 *  Created on: Oct 30, 2017
 *      Author: lion
 */

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "utils.h"

namespace fibjs {

extern int32_t stack_size;

extern bool g_prof;
extern int32_t g_prof_interval;

extern bool g_cov;
}

#endif