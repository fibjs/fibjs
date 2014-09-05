#include "v8.h"

#include "platform.h"
#include <exlib/include/fiber.h>

#ifdef _WIN32

#ifdef CreateMutex
#undef CreateMutex
#endif

#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

#endif

#if 1

inline int64_t getTime();

namespace v8
{
namespace base
{

class Thread::PlatformData
{
public:
	PlatformData() :
			thread_(NULL)
	{
	}

	exlib::Fiber* thread_; // Thread handle for pthread.
};

Thread::Thread(const Options& options) :
		data_(new PlatformData()), stack_size_(options.stack_size())
{
	set_name(options.name());
}

Thread::~Thread()
{
	data_->thread_->Unref();
	delete data_;
}

static void* ThreadEntry(void* arg)
{
	Thread* thread = reinterpret_cast<Thread*>(arg);

	thread->data()->thread_ = exlib::Fiber::Current();
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
	data_->thread_ = exlib::Service::CreateFiber(ThreadEntry, this);
}

void Thread::Join()
{
	data_->thread_->join();
}

Thread::LocalStorageKey Thread::CreateThreadLocalKey()
{
	return static_cast<LocalStorageKey>(exlib::Service::tlsAlloc());
}

void Thread::DeleteThreadLocalKey(LocalStorageKey key)
{
	exlib::Service::tlsFree(static_cast<int>(key));
}

void* Thread::GetThreadLocal(LocalStorageKey key)
{
	return exlib::Service::tlsGet(static_cast<int>(key));
}

void Thread::SetThreadLocal(LocalStorageKey key, void* value)
{
	exlib::Service::tlsPut(static_cast<int>(key), value);
}

void Thread::YieldCPU()
{
	exlib::Fiber::yield();
}

void OS::Sleep(int milliseconds)
{
	exlib::Fiber::sleep(milliseconds);
}

}

}

#endif
