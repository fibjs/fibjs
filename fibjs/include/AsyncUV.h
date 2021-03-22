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

class AsyncUVFS : public uv_fs_t {
public:
    AsyncUVFS(AsyncEvent* ac)
        : m_ac(ac)
    {
    }

public:
    static void callback(uv_fs_t* req)
    {
        AsyncUVFS* pThis = (AsyncUVFS*)req;

        int ret = uv_fs_get_result(req);
        if (ret == 0)
            pThis->m_ac->apost(0);
        else
            pThis->m_ac->apost(Runtime::setError(uv_strerror(ret)));

        uv_fs_req_cleanup(req);
        delete pThis;
    }

private:
    AsyncEvent* m_ac;
};

class AsyncUVFSResult : public uv_fs_t {
public:
    AsyncUVFSResult(exlib::string& retVal, AsyncEvent* ac)
        : m_retVal(retVal)
        , m_ac(ac)
    {
    }

public:
    static void callback(uv_fs_t* req)
    {
        AsyncUVFSResult* pThis = (AsyncUVFSResult*)req;

        int ret = uv_fs_get_result(req);
        if (ret == 0) {
            pThis->m_retVal = (const char*)pThis->ptr;
            pThis->m_ac->apost(0);
        } else
            pThis->m_ac->apost(Runtime::setError(uv_strerror(ret)));

        uv_fs_req_cleanup(req);
        delete pThis;
    }

private:
    exlib::string& m_retVal;
    AsyncEvent* m_ac;
};

extern uv_loop_t* s_uv_loop;
}
