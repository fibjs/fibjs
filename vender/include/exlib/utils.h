/*
 *  utils.h
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "../osconfig.h"
#include <stdint.h>

#ifndef _ex_utils_h__
#define _ex_utils_h__

#ifdef WIN32
#include "utils_win.h"
#elif defined(x64)
#include "utils_x64.h"
#elif defined(I386)
#include "utils_x86.h"
#elif defined(arm)
#include "utils_arm.h"
#endif

#endif
