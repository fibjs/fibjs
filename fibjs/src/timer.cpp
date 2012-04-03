#include <exlib/thread.h>
#include <exlib/lockfree.h>
#include "AsyncCall.h"

#include <map>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>

static LARGE_INTEGER systemFrequency;

inline int64_t Ticks()
{
    LARGE_INTEGER t;

    if(systemFrequency.QuadPart == 0)
        QueryPerformanceFrequency(&systemFrequency);

    QueryPerformanceCounter(&t);

    return t.QuadPart * 1000000 / systemFrequency.QuadPart;
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
        return 0;
    return (tv.tv_sec * 1000000ll) + tv.tv_usec;
}

#endif

namespace fibjs
{

exlib::lockfree<AsyncCall> s_acReady;
exlib::lockfree<AsyncCall> s_acs;

std::multimap<int64_t, AsyncCall*> s_tms;
static int64_t s_time;

class timerThread : public exlib::Thread
{
public:
    timerThread()
    {
        s_time = Ticks();
        Start();
    }

    virtual void Run()
    {
        AsyncCall *p;
        int64_t tm;
        std::multimap <int64_t, AsyncCall*>::iterator e;

        while(1)
        {
            Sleep(1);

            while(1)
            {
                p = s_acs.get();
                if(p == NULL)
                    break;

                tm = s_time + *(int*)p->args[0] * 1000;
                s_tms.insert(std::make_pair(tm, p));
            }

            s_time = Ticks();

            while(1)
            {
                e = s_tms.begin();
                if(e == s_tms.end())
                    break;
                if(e->first > s_time)
                    break;

                s_acReady.put(e->second);
                s_tms.erase(e);
            }
        }
    }
};

static timerThread s_timer;

void on_async_ready()
{
    AsyncCall *p;

    while(1)
    {
        p = s_acReady.get();
        if(p == NULL)
            break;

        p->weak.set();
    }
}

}
