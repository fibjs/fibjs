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

// The uv junction (E4 of the strict audit): the raw uv status crosses it as
// int32_t -- the degraded form -- and uv_call frames it once (from_system);
// anything else is success. A callable that has its own context to describe
// does not build a value inside: the call site re-codes the framed value and
// adds it (UVSocket::create / ChildProcess::spawn).
result_t uv_call(std::function<int32_t(void)> proc);
inline result_t uv_async(std::function<int32_t(void)> proc)
{
    result_t ret = uv_call(std::move(proc));

    return ret.failed() ? ret : result_t::pending();
}

class AutoReq : public uv_fs_t {
public:
    ~AutoReq()
    {
        uv_fs_req_cleanup(this);
    }
};

}
