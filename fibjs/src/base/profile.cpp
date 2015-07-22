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

extern exlib::LockedList<Isolate> s_isolates;

static std::string traceFiber(bool bTraceNow)
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

            if (bTraceNow && fb == now && fb->m_traceInfo.empty())
                msg.append(traceInfo(300));
            else
                msg.append(fb->m_traceInfo);
            p = p->m_next;
        }
    } else
        msg.append(traceInfo(300));

    return msg;
}

static void dumpFibers(bool bTraceNow)
{
    std::string msg;

    msg.append(COLOR_LIGHTRED "User interrupt.");
    msg.append(traceFiber(bTraceNow));
    msg.append(COLOR_RESET "\n");

    std_logger::out(msg.c_str());
    _exit(1);
}

static void InterruptCallback(v8::Isolate *isolate, void *data)
{
    dumpFibers(true);
}

static void InterruptCallbackEx()
{
    if (Isolate::rt::g_trace)
        dumpFibers(false);

    Isolate &is = Isolate::now();

    if (!JSFiber::current())
        is.service->RequestInterrupt(InterruptCallbackEx);
    else
    {
        v8::Locker locker(is.isolate);
        dumpFibers(false);
    }
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

    Isolate *p = s_isolates.head();
    while (p != 0) {
#ifdef DEBUG
        p->service->dumpFibers();
        exlib::mem_diff();
#endif
        p->isolate->RequestInterrupt(InterruptCallback, NULL);
        p->service->RequestInterrupt(InterruptCallbackEx);

        p = s_isolates.next(p);
    }
}

}
