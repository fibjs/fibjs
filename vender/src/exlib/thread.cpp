/*
 *  thread.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/thread.h>
#include <stack>

namespace exlib
{

static void *ThreadEntry(void *arg)
{
    OSThread *thread = reinterpret_cast<OSThread *>(arg);
    thread->Run();
    return NULL;
}

OSThread::OSThread() : thread_(0)
{
}

OSThread::~OSThread()
{
    detach();
}

#ifdef _WIN32

OSSemaphore::OSSemaphore(int start_val)
{
    m_handle = ::CreateSemaphore(NULL, start_val, 1, NULL);
}

void OSThread::detach()
{
    if (thread_)
        CloseHandle(thread_);
    thread_ = NULL;
}

void OSThread::start()
{
    thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, this, 0, &threadid);
}

void OSThread::join()
{
    WaitForSingleObject(thread_, INFINITE);
}

OSCondVarOld::OSCondVarOld(OSMutex *mu)
    : user_lock_(*mu),
      run_state_(RUNNING),
      allocation_counter_(0),
      recycling_list_size_(0)
{
}

#ifndef Scoped_Lock_Protect
#define Scoped_Lock_Protect(mu) AutoLock __auto_lock__(mu)
#endif

#ifndef Scoped_Unlock_Protect
#define Scoped_Unlock_Protect(mu) AutoUnlock __auto_unlock__(mu)
#endif

OSCondVarOld::~OSCondVarOld()
{
    Scoped_Lock_Protect(internal_lock_);
    run_state_ = SHUTDOWN;
    if (recycling_list_size_ != allocation_counter_)
    {
        Scoped_Unlock_Protect(internal_lock_);
        SignalAll();
        Sleep(10);
    }
}

void OSCondVarOld::TimedWait(DWORD dwMilliseconds)
{
    Event *waiting_event;
    HANDLE handle;
    {
        Scoped_Lock_Protect(internal_lock_);
        if (RUNNING != run_state_) return;
        waiting_event = GetEventForWaiting();
        handle = waiting_event->handle();
    }

    {
        Scoped_Unlock_Protect(user_lock_);
        WaitForSingleObject(handle, dwMilliseconds);

        Scoped_Lock_Protect(internal_lock_);
        RecycleEvent(waiting_event);
    }
}

void OSCondVarOld::SignalAll()
{
    std::stack<HANDLE> handles;
    {
        Scoped_Lock_Protect(internal_lock_);
        if (waiting_list_.IsEmpty())
            return;
        while (!waiting_list_.IsEmpty())
            handles.push(waiting_list_.PopBack()->handle());
    }
    while (!handles.empty())
    {
        SetEvent(handles.top());
        handles.pop();
    }
}

void OSCondVarOld::Signal()
{
    HANDLE handle;
    {
        Scoped_Lock_Protect(internal_lock_);
        if (waiting_list_.IsEmpty())
            return;
        handle = waiting_list_.PopBack()->handle();
    }
    SetEvent(handle);
}

OSCondVarOld::Event *OSCondVarOld::GetEventForWaiting()
{
    Event *cv_event;
    if (0 == recycling_list_size_)
    {
        cv_event = new Event();
        cv_event->InitListElement();
        allocation_counter_++;
    }
    else
    {
        cv_event = recycling_list_.PopFront();
        recycling_list_size_--;
    }
    waiting_list_.PushBack(cv_event);
    return cv_event;
}

void OSCondVarOld::RecycleEvent(Event *used_event)
{
    used_event->Extract();
    recycling_list_.PushBack(used_event);
    recycling_list_size_++;
}

OSCondVarOld::Event::Event() : handle_(0)
{
    next_ = prev_ = this;
}

OSCondVarOld::Event::~Event()
{
    if (0 == handle_)
    {
        while (!IsEmpty())
        {
            Event *cv_event = PopFront();
            delete cv_event;
        }
    }
    if (0 != handle_)
    {
        int ret_val = CloseHandle(handle_);
    }
}

void OSCondVarOld::Event::InitListElement()
{
    handle_ = CreateEvent(NULL, false, false, NULL);
}

bool OSCondVarOld::Event::IsEmpty() const
{
    return IsSingleton();
}

void OSCondVarOld::Event::PushBack(Event *other)
{

    other->prev_ = prev_;
    other->next_ = this;

    prev_->next_ = other;
    prev_ = other;
}

OSCondVarOld::Event *OSCondVarOld::Event::PopFront()
{
    return next_->Extract();
}

OSCondVarOld::Event *OSCondVarOld::Event::PopBack()
{
    return prev_->Extract();
}

HANDLE OSCondVarOld::Event::handle() const
{
    return handle_;
}

OSCondVarOld::Event *OSCondVarOld::Event::Extract()
{
    if (!IsSingleton())
    {
        next_->prev_ = prev_;
        prev_->next_ = next_;
        prev_ = next_ = this;
    }
    return this;
}

bool OSCondVarOld::Event::IsSingleton() const
{
    return next_ == this;
}

bool OSCondVarOld::Event::ValidateAsDistinct(Event *other) const
{
    return ValidateLinks() && other->ValidateLinks() && (this != other);
}

bool OSCondVarOld::Event::ValidateAsItem() const
{
    return (0 != handle_) && ValidateLinks();
}

bool OSCondVarOld::Event::ValidateAsList() const
{
    return (0 == handle_) && ValidateLinks();
}

bool OSCondVarOld::Event::ValidateLinks() const
{
    return (next_->prev_ == this) && (prev_->next_ == this);
}

typedef VOID (WINAPI *CondProc)(PCONDITION_VARIABLE);

static CondProc pInitializeConditionVariable;
static CondProc pWakeAllConditionVariable;
static CondProc pWakeConditionVariable;
static BOOL (WINAPI *pSleepConditionVariableCS)(PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD);

OSCondVarNew::OSCondVarNew(OSMutex *mu) : _mu(mu)
{
    InitializeConditionVariable(&_cv);
}

OSCondVarNew::~OSCondVarNew()
{
    WakeAllConditionVariable(&_cv);
}

void OSCondVarNew::Wait()
{
    SleepConditionVariableCS(&_cv, &_mu->cs_, INFINITE);
}

void OSCondVarNew::Signal()
{
    WakeConditionVariable(&_cv);
}

void OSCondVarNew::SignalAll()
{
    WakeAllConditionVariable(&_cv);
}

static class _initConv
{
public:
    _initConv()
    {
        HMODULE hKernel = GetModuleHandleA("KERNEL32");

        if (hKernel)
        {
            pInitializeConditionVariable = (CondProc)GetProcAddress(hKernel, "InitializeConditionVariable");
            pWakeAllConditionVariable = (CondProc)GetProcAddress(hKernel, "WakeAllConditionVariable");
            pWakeConditionVariable = (CondProc)GetProcAddress(hKernel, "WakeConditionVariable");
            pSleepConditionVariableCS = (BOOL (WINAPI *)(PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD))
                                        GetProcAddress(hKernel, "SleepConditionVariableCS");

            if (!pInitializeConditionVariable || !pWakeAllConditionVariable ||
                    !pWakeConditionVariable || !pSleepConditionVariableCS)
            {
                pInitializeConditionVariable = NULL;
                pWakeAllConditionVariable = NULL;
                pWakeConditionVariable = NULL;
                pSleepConditionVariableCS = NULL;
            }
        }

    }
} s_initConv;

OSCondVar::OSCondVar(OSMutex *mu)
{
    if (pInitializeConditionVariable)
        _cd = new OSCondVarNew(mu);
    else
        _cd = new OSCondVarOld(mu);
}

OSCondVar::~OSCondVar()
{
    if (pInitializeConditionVariable)
        delete (OSCondVarNew *)_cd;
    else
        delete (OSCondVarOld *)_cd;

    _cd = NULL;
}

void OSCondVar::Wait()
{
    if (pInitializeConditionVariable)
        ((OSCondVarNew *)_cd)->Wait();
    else
        ((OSCondVarOld *)_cd)->Wait();
}

void OSCondVar::Signal()
{
    if (pInitializeConditionVariable)
        ((OSCondVarNew *)_cd)->Signal();
    else
        ((OSCondVarOld *)_cd)->Signal();
}

void OSCondVar::SignalAll()
{
    if (pInitializeConditionVariable)
        ((OSCondVarNew *)_cd)->SignalAll();
    else
        ((OSCondVarOld *)_cd)->SignalAll();
}

#else

void OSThread::detach()
{
    if (thread_)
        pthread_detach(thread_);
    thread_ = (pthread_t)NULL;
}

void OSThread::start()
{
    pthread_create(&thread_, NULL, ThreadEntry, this);
}

void OSThread::join()
{
    pthread_join(thread_, NULL);
}

#endif

}

