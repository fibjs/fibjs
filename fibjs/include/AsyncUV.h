/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-26 19:00:27
 * @modify date 2020-06-26 19:00:27
 * @desc general AsyncUV Item Runnin in AsyncUVLoopThread
 */

#pragma once

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
inline int uv_async(std::function<int(void)> proc)
{
    int ret = uv_call(proc);
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

extern uv_loop_t* s_uv_loop;
}
