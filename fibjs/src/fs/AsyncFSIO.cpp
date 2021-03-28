#include "object.h"
#include "utils.h"
#include "AsyncUV.h"
#include "StatsWatcher.h"
#include <uv/include/uv.h>
#include "Runtime.h"
#include "Buffer.h"

namespace fibjs {

uv_loop_t* s_uv_loop;
exlib::LockedList<AsyncEvent> s_uv_tasks;

class UVAsyncThread : public exlib::OSThread {
public:
    UVAsyncThread()
    {
        start();
    }

public:
    virtual void Run()
    {
        Runtime rtForThread(NULL);

        s_uv_loop = new uv_loop_t();
        uv_loop_init(s_uv_loop);

        uv_async_init(s_uv_loop, &m_uv_async, AsyncEventCallback);

        m_init.set();

        uv_run(s_uv_loop, UV_RUN_DEFAULT);
    }

    void post(AsyncEvent* task)
    {
        m_init.wait();
        s_uv_tasks.putTail(task);
        uv_async_send(&m_uv_async);
    }

private:
    static void AsyncEventCallback(uv_async_t* handle)
    {
        assert(handle == &s_uv_async);

        exlib::List<AsyncEvent> jobs;
        AsyncEvent* p1;

        s_uv_tasks.getList(jobs);

        while ((p1 = jobs.getHead()) != 0)
            p1->invoke();
    }

public:
    exlib::Event m_init;

private:
    uv_async_t m_uv_async;
};

static UVAsyncThread* s_afsIO;

int uv_call(std::function<int(void)> proc)
{
    class UVCall : public AsyncEvent {
    public:
        UVCall(std::function<int(void)>& proc)
            : AsyncEvent(NULL)
            , m_proc(proc)
        {
            s_afsIO->post(this);
            m_event.wait();
        }

        void invoke()
        {
            m_res = m_proc();
            m_event.set();
        }

    public:
        std::function<int(void)> m_proc;
        int m_res;
        exlib::Event m_event;
    };

    UVCall uvc(proc);
    return uvc.m_res;
}

void initializeUVAsyncThread()
{
    s_afsIO = new UVAsyncThread();
}
}