#include "v8.h"

#include <exlib/fiber.h>

#include "platform.h"
#include "v8threads.h"
#include "vm-state-inl.h"

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
namespace internal
{

class Thread::PlatformData: public Malloced
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
}

static void* ThreadEntry(void* arg)
{
	Thread* thread = reinterpret_cast<Thread*>(arg);

	thread->data()->thread_ = exlib::Fiber::Current();
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

class FiberMutex: public Mutex
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
	exlib::Locker _mutex;
};

Mutex* OS::CreateMutex()
{
	return new FiberMutex();
}

class FiberSemaphore: public Semaphore
{
public:
	FiberSemaphore(int count) :
			sem_(count)
	{
	}
	virtual void Wait();
	virtual bool Wait(int timeout);
	virtual void Signal()
	{
		sem_.post();
	}
private:
	exlib::Semaphore sem_;
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

class Sampler::PlatformData: public Malloced
{
public:
	PlatformData() :
			m_pFiber(exlib::Fiber::Current())
	{
		m_pFiber->Ref();
	}

	~PlatformData()
	{
		m_pFiber->Unref();
	}

	exlib::Fiber* profiled_thread()
	{
		return m_pFiber;
	}

private:
	// Note: for profiled_thread_ Mach primitives are used instead of PThread's
	// because the latter doesn't provide thread manipulation primitives required.
	// For details, consult "Mac OS X Internals" book, Section 7.3.
	exlib::Fiber* m_pFiber;
};

class SamplerThread: public Thread
{
public:
	static const int kSamplerThreadStackSize = 64 * KB;

	explicit SamplerThread(int interval) :
			Thread(Thread::Options("SamplerThread", kSamplerThreadStackSize)), interval_(
					interval)
	{
	}

	static void SetUp()
	{
		if (!mutex_)
			mutex_ = OS::CreateMutex();
	}
	static void TearDown()
	{
		delete mutex_;
	}

	static void AddActiveSampler(Sampler* sampler)
	{
		ScopedLock lock(mutex_);
		SamplerRegistry::AddActiveSampler(sampler);
		if (instance_ == NULL)
		{
			instance_ = new SamplerThread(sampler->interval());
			instance_->Start();
		}
		else
		{
			ASSERT(instance_->interval_ == sampler->interval());
		}
	}

	static void RemoveActiveSampler(Sampler* sampler)
	{
		ScopedLock lock(mutex_);
		SamplerRegistry::RemoveActiveSampler(sampler);
		if (SamplerRegistry::GetState() == SamplerRegistry::HAS_NO_SAMPLERS)
		{
			RuntimeProfiler::StopRuntimeProfilerThreadBeforeShutdown(instance_);
			delete instance_;
			instance_ = NULL;
		}
	}

	// Implement Thread::Run().
	virtual void Run()
	{
		SamplerRegistry::State state;
		while ((state = SamplerRegistry::GetState())
				!= SamplerRegistry::HAS_NO_SAMPLERS)
		{
			OS::Sleep(500);
			bool cpu_profiling_enabled = (state
					== SamplerRegistry::HAS_CPU_PROFILING_SAMPLERS);
			bool runtime_profiler_enabled = RuntimeProfiler::IsEnabled();
			// When CPU profiling is enabled both JavaScript and C++ code is
			// profiled. We must not suspend.
			if (!cpu_profiling_enabled)
			{
				if (rate_limiter_.SuspendIfNecessary())
					continue;
			}
			if (cpu_profiling_enabled)
			{
				if (!SamplerRegistry::IterateActiveSamplers(&DoCpuProfile,
						this))
				{
					return;
				}
			}
			if (runtime_profiler_enabled)
			{
				if (!SamplerRegistry::IterateActiveSamplers(&DoRuntimeProfile,
						NULL))
				{
					return;
				}
			}
			OS::Sleep(interval_);
		}
	}

	static void DoCpuProfile(Sampler* sampler, void* raw_sampler_thread)
	{
		if (!sampler->isolate()->IsInitialized())
			return;
		if (!sampler->IsProfiling())
			return;
		SamplerThread* sampler_thread =
				reinterpret_cast<SamplerThread*>(raw_sampler_thread);
		sampler_thread->SampleContext(sampler);
	}

	static void DoRuntimeProfile(Sampler* sampler, void* ignored)
	{
		if (!sampler->isolate()->IsInitialized())
			return;
		sampler->isolate()->runtime_profiler()->NotifyTick();
	}

	void SampleContext(Sampler* sampler)
	{
		exlib::Fiber* pFiber = exlib::Fiber::Current();
		TickSample sample_obj;
		TickSample* sample = CpuProfiler::TickSampleEvent(sampler->isolate());
		if (sample == NULL)
			sample = &sample_obj;

		sample->state = sampler->isolate()->current_vm_state();

#if defined(x64)
		sample->pc = reinterpret_cast<Address>(pFiber->m_cntxt.Rip);
		sample->sp = reinterpret_cast<Address>(pFiber->m_cntxt.Rsp);
		sample->fp = reinterpret_cast<Address>(pFiber->m_cntxt.Rbp);
#else
		sample->pc = reinterpret_cast<Address>(pFiber->m_cntxt.Eip);
		sample->sp = reinterpret_cast<Address>(pFiber->m_cntxt.Esp);
		sample->fp = reinterpret_cast<Address>(pFiber->m_cntxt.Ebp);
#endif

		sampler->SampleStack(sample);
		sampler->Tick(sample);
	}

	const int interval_;
	RuntimeProfilerRateLimiter rate_limiter_;

	// Protects the process wide state below.
	static Mutex* mutex_;
	static SamplerThread* instance_;

private:
	DISALLOW_COPY_AND_ASSIGN(SamplerThread);
};

#undef REGISTER_FIELD

Mutex* SamplerThread::mutex_ = NULL;
SamplerThread* SamplerThread::instance_ = NULL;
extern Mutex* limit_mutex;

void OS::SetUp()
{
#ifdef _WIN32
	uint64_t seed = static_cast<uint64_t>(TimeCurrentMillis());
	srand(static_cast<unsigned int>(seed));
#else
	uint64_t seed = Ticks() ^ (getpid() << 16);
	srandom(static_cast<unsigned int>(seed));
#endif

	limit_mutex = CreateMutex();
	SamplerThread::SetUp();
}

void OS::TearDown()
{
	SamplerThread::TearDown();
	delete limit_mutex;
}

Sampler::Sampler(Isolate* isolate, int interval) :
		isolate_(isolate), interval_(interval), profiling_(false), active_(
				false), samples_taken_(0)
{
	data_ = new PlatformData;
}

Sampler::~Sampler()
{
	ASSERT(!IsActive());
	delete data_;
}

void Sampler::Start()
{
	ASSERT(!IsActive());
	SetActive(true);
	SamplerThread::AddActiveSampler(this);
}

void Sampler::Stop()
{
	ASSERT(IsActive());
	SamplerThread::RemoveActiveSampler(this);
	SetActive(false);
}

void OS::Sleep(int milliseconds)
{
	exlib::Fiber::sleep(milliseconds);
}

}

}

#endif
