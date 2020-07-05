/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-26 19:00:27
 * @modify date 2020-06-26 19:00:27
 * @desc general AsyncUV Item Runnin in AsyncUVLoopThread
 */

#ifndef ASYNCUV_H_
#define ASYNCUV_H_

#include "AsyncCall.h"

namespace fibjs {
    class AsyncUVTask: public AsyncEvent
    {
    public:
        AsyncUVTask(): AsyncEvent(NULL)
        {
        };
        ~AsyncUVTask()
        {
        };
    };

}

#endif // ASYNCUV_H_