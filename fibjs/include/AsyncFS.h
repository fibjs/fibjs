/*
 * AsyncFS.h
 *
 *  Created on: Mar 27, 2021
 *      Author: lion
 */

#pragma once

#include "AsyncUV.h"
#include "utils.h"
#include "Stat.h"

namespace fibjs {
class AsyncUVFS : public uv_fs_t {
public:
    AsyncUVFS(AsyncEvent* ac)
        : m_ac(ac)
    {
    }

    ~AsyncUVFS()
    {
        uv_fs_req_cleanup(this);
    }

public:
    static void callback(uv_fs_t* req)
    {
        AsyncUVFS* pThis = (AsyncUVFS*)req;

        pThis->m_ac->apost(uv_fs_get_result(req));
        delete pThis;
    }

private:
    AsyncEvent* m_ac;
};

inline void uv_fs_get_value(const uv_fs_t* pThis, exlib::string& retVal)
{
    retVal = (const char*)pThis->ptr;
}

inline void uv_fs_get_value(const uv_fs_t* pThis, obj_ptr<Stat_base>& retVal)
{
    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(pThis->path, &pThis->statbuf);
    retVal = pStat;
}

template <typename T>
class AsyncUVFSResult : public uv_fs_t {
public:
    AsyncUVFSResult(T& retVal, AsyncEvent* ac)
        : m_retVal(retVal)
        , m_ac(ac)
    {
    }

    ~AsyncUVFSResult()
    {
        uv_fs_req_cleanup(this);
    }

public:
    static void callback(uv_fs_t* req)
    {
        AsyncUVFSResult* pThis = (AsyncUVFSResult*)req;

        int32_t ret = uv_fs_get_result(req);
        if (ret == 0)
            uv_fs_get_value(req, pThis->m_retVal);

        pThis->m_ac->apost(ret);
        delete pThis;
    }

private:
    T& m_retVal;
    AsyncEvent* m_ac;
};
}