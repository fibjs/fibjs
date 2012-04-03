
#include <exlib/thread.h>

namespace fibjs
{

class timerThread : public exlib::Thread
{
public:
    timerThread()
    {
        Start();
    }

    virtual void Run()
    {
        while(1)
        {
            Sleep(1);
        }
    }
};

static timerThread s_timer;




}
