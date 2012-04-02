#include <fiber.h>
#include "v8.h"

#include "platform.h"
#include "v8threads.h"
#include "vm-state-inl.h"

#if 1

inline int64_t getTime();

namespace v8
{
namespace internal
{


class Thread::PlatformData : public Malloced
{
public:
    PlatformData() : thread_(NULL) {}

    fiber::Fiber* thread_;  // Thread handle for pthread.
};

Thread::Thread(const Options& options)
    : data_(new PlatformData()),
    stack_size_(options.stack_size())
{
    set_name(options.name());
}


Thread::~Thread()
{
}


static void* ThreadEntry(void* arg)
{
    Thread* thread = reinterpret_cast<Thread*>(arg);

    thread->data()->thread_ = fiber::Fiber::Current();
    ASSERT(thread->data()->thread_ != NULL);
    thread->Run();
    return NULL;
}


void Thread::set_name(const char* name)
{
    strncpy(name_, name, sizeof(name_));
    name_[sizeof(name_) - 1] = '\0';
}


void Thread::Start()
{
    data_->thread_ = fiber::Service::CreateFiber(ThreadEntry, this);
}


void Thread::Join()
{
    data_->thread_->join();
}



Thread::LocalStorageKey Thread::CreateThreadLocalKey()
{
    return static_cast<LocalStorageKey>(fiber::Service::tlsAlloc());
}

void Thread::DeleteThreadLocalKey(LocalStorageKey key)
{
    fiber::Service::tlsFree(static_cast<int>(key));
}


void* Thread::GetThreadLocal(LocalStorageKey key)
{
    return fiber::Service::tlsGet(static_cast<int>(key));
}


void Thread::SetThreadLocal(LocalStorageKey key, void* value)
{
    fiber::Service::tlsPut(static_cast<int>(key), value);
}



void Thread::YieldCPU()
{
    fiber::Fiber::yield();
}


class FiberMutex : public Mutex
{
public:

    virtual int Lock()
    {
        _mutex.lock();
        return 0;
    }

    virtual int Unlock()
    {
        _mutex.unlock();
        return 0;
    }

    virtual bool TryLock()
    {
        return _mutex.trylock();
    }

private:
    fiber::Locker _mutex;
};


Mutex* OS::CreateMutex()
{
    return new FiberMutex();
}


class FiberSemaphore : public Semaphore
{
public:
    FiberSemaphore(int count) : sem_(count) {}
    virtual void Wait();
    virtual bool Wait(int timeout);
    virtual void Signal()
    {
        sem_.post();
    }
private:
    fiber::Semaphore sem_;
};


void FiberSemaphore::Wait()
{
    sem_.wait();
}


bool FiberSemaphore::Wait(int timeout)
{
    sem_.wait();
    return true;
}


Semaphore* OS::CreateSemaphore(int count)
{
    return new FiberSemaphore(count);
}

static int64_t s_time;

double OS::TimeCurrentMillis()
{
    return getTime() / 1000.0;
}

int64_t OS::Ticks()
{
    return getTime();
}

}
}

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
inline int64_t getTime()
{
    union TimeStamp
    {
        FILETIME ft_;
        int64_t t_;
    };

    static bool initialized = false;
    static TimeStamp init_time;
    static DWORD init_ticks;
    static const int64_t kHundredNanosecondsPerSecond = 10000000;
    static const int64_t kMaxClockElapsedTime =
        60*kHundredNanosecondsPerSecond;  // 1 minute

    // If we are uninitialized, we need to resync the clock.
    bool needs_resync = !initialized;

    // Get the current time.
    TimeStamp time_now;
    GetSystemTimeAsFileTime(&time_now.ft_);
    DWORD ticks_now = timeGetTime();

    // Check if we need to resync due to clock rollover.
    needs_resync |= ticks_now < init_ticks;

    // Check if we need to resync due to elapsed time.
    needs_resync |= (time_now.t_ - init_time.t_) > kMaxClockElapsedTime;

    // Check if we need to resync due to backwards time change.
    needs_resync |= time_now.t_ < init_time.t_;

    // Resync the clock if necessary.
    if (needs_resync)
    {
        GetSystemTimeAsFileTime(&init_time.ft_);
        init_ticks = ticks_now = timeGetTime();
        initialized = true;
    }

    // Finally, compute the actual time.  Why is this so hard.
    DWORD elapsed = ticks_now - init_ticks;
    return init_time.t_ + (static_cast<int64_t>(elapsed) * 10000);

}

#else
#include <sys/time.h>

inline int64_t getTime()
{
    // gettimeofday has microsecond resolution.
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
        return 0;
    return (static_cast<int64_t>(tv.tv_sec) * 1000000) + tv.tv_usec;
}

#endif

#endif
