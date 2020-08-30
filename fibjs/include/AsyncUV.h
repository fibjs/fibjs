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
#include <uv/include/uv.h>
#include <functional>

namespace fibjs {
class AsyncUVTask : public AsyncEvent {
public:
    AsyncUVTask()
        : AsyncEvent(NULL) {};
    ~AsyncUVTask() {};
};

int uv_call(std::function<int(void)> proc);

extern uv_loop_t* s_uv_loop;
}

#endif // ASYNCUV_H_