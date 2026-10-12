#include "object.h"
#include "utils.h"
#include "AsyncUV.h"
#include <uv/include/uv.h>
#include "Runtime.h"
#include "Buffer.h"

namespace fibjs {

uv_loop_t* s_uv_loop;
exlib::LockedList<AsyncEvent> s_uv_tasks;

uv_loop_s* Isolate::event_loop()
{
    return s_uv_loop;
}

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

void uv_post(AsyncEvent* task)
{
    s_afsIO->post(task);
}

void uv_post(std::function<void(void)> proc)
{
    class UVPost : public AsyncEvent {
    public:
        UVPost(std::function<void(void)>& proc)
            : AsyncEvent(NULL)
            , m_proc(proc)
        {
        }

        void invoke()
        {
            m_proc();
            delete this;
        }

    public:
        std::function<void(void)> m_proc;
    };

    uv_post(new UVPost(proc));
}

result_t uv_call(std::function<int32_t(void)> proc)
{
    class UVCall : public AsyncEvent {
    public:
        UVCall(std::function<int32_t(void)>& proc)
            : AsyncEvent(NULL)
            , m_proc(proc)
        {
            uv_post(this);
            m_event.wait();
        }

        void invoke()
        {
            // The junction frames the raw status here, once: a negative uv
            // code is -errno (a system error), anything else is success.
            int32_t ret = m_proc();

            if (ret < 0)
                m_res = result_t::from_system(ret);
            else
                m_res = result_t();

            m_event.set();
        }

    public:
        std::function<int32_t(void)> m_proc;
        result_t m_res;
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