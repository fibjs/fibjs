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
extern uv_loop_t* s_uv_loop;

void uv_post(AsyncEvent* task);
void uv_post(std::function<void(void)> proc);

int uv_call(std::function<int(void)> proc);
inline int uv_async(std::function<int(void)> proc)
{
    int ret = uv_call(proc);
    return ret ? ret : CALL_E_PENDDING;
}

class AutoReq : public uv_fs_t {
public:
    ~AutoReq()
    {
        uv_fs_req_cleanup(this);
    }
};

}
