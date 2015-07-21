/*
 * profile.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: lion
 */

#include "console.h"
#include "Fiber.h"

namespace fibjs
{

extern exlib::lockfree<Isolate> s_isolates;

static std::string traceFiber()
{
    JSFiber* now = JSFiber::current();
    std::string msg;

    if (Isolate::rt::g_trace)
    {
        exlib::linkitem* p = Isolate::now().m_fibers.head();

        char buf[128];
        int32_t n = 0;

        while (p)
        {
            JSFiber* fb = (JSFiber*)p;

            sprintf(buf, "\nFiber %d:", n++);
            msg.append(buf);

            if (fb == now && fb->m_traceInfo.empty())
                msg.append(traceInfo(300));
            else
                msg.append(fb->m_traceInfo);
            p = p->m_next;
        }
    } else
        msg.append(traceInfo(300));

    return msg;
}

static void InterruptCallback(v8::Isolate *isolate, void *data)
{
    std::string msg;

    msg.append(COLOR_LIGHTRED "User interrupt.");
    msg.append(traceFiber());
    msg.append(COLOR_RESET "\n");

    std_logger::out(msg.c_str());
    _exit(1);
}

static void InterruptCallbackEx()
{
    JSFiber* now = JSFiber::current();
    Isolate &is = Isolate::now();

    if (!now || now->m_traceInfo.empty())
        is.service->RequestInterrupt(InterruptCallbackEx);
    else
        InterruptCallback(is.isolate, NULL);
}

void userBreak()
{
    puts("");

    static bool s_double = false;
    if (s_double)
    {
        puts("User interrupt.");
        _exit(1);
    }
    s_double = true;

    Isolate *p;
    while ((p = s_isolates.get()) != 0) {
#ifdef DEBUG
        p->service->dumpFibers();
        exlib::mem_diff();
#endif
        p->isolate->RequestInterrupt(InterruptCallback, NULL);
        p->service->RequestInterrupt(InterruptCallbackEx);
    }
}

}
