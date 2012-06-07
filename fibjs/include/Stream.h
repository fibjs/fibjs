/*
 * Stream.h
 *
 *  Created on: Jun 6, 2012
 *      Author: lion
 */

#include "ifs/Stream.h"

#ifndef STREAM_H_
#define STREAM_H_

namespace fibjs
{

result_t copyStream(Stream_base* from, Stream_base* to, int32_t bytes,
		int32_t& retVal, exlib::AsyncEvent* ac);

}

#endif /* STREAM_H_ */
