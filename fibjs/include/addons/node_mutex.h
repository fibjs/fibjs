#pragma once

#include <mutex>
#include <queue>
#include <exlib/include/fiber.h>

namespace node {
class Mutex {
public:
    void Lock()
    {
        m_locker.lock();
    }

    void Unlock()
    {
        m_locker.unlock();
    }

public:
    class ScopedLock {
    public:
        ScopedLock(Mutex& mutex)
            : m_mutex(mutex)
        {
            m_mutex.Lock();
        }
        ~ScopedLock() { m_mutex.Unlock(); }

    public:
        Mutex& m_mutex;
    };

public:
    exlib::Locker m_locker;
};

class ConditionVariable {
public:
    void Signal(const Mutex::ScopedLock& scoped_lock)
    {
        m_condvar.notify_one();
    }

    void Wait(const Mutex::ScopedLock& scoped_lock)
    {
        m_condvar.wait(scoped_lock.m_mutex.m_locker);
    }

private:
    exlib::CondVar m_condvar;
};

} // namespace node
