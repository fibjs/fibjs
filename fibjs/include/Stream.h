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

result_t copyStream(Stream_base *from, Stream_base *to, int64_t bytes,
                    int64_t &retVal, AsyncEvent *ac);

#define STREAM_BUFF_SIZE    65536

}

#endif /* STREAM_H_ */
