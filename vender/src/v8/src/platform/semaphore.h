#ifndef V8_PLATFORM_SEMAPHORE_H_
#define V8_PLATFORM_SEMAPHORE_H_

#include <exlib/fiber.h>
#include "lazy-instance.h"

namespace v8
{
namespace internal
{

class TimeDelta;

class Semaphore
V8_FINAL
{
	public:
		explicit Semaphore(int count);
		~Semaphore();

		void Signal();
		void Wait();bool WaitFor(const TimeDelta& rel_time)
				V8_WARN_UNUSED_RESULT;

		typedef exlib::Semaphore NativeHandle;

		NativeHandle& native_handle()
		{
			return native_handle_;
		}
		const NativeHandle& native_handle() const
		{
			return native_handle_;
		}

	private:
		NativeHandle native_handle_;

		DISALLOW_COPY_AND_ASSIGN(Semaphore);
	};

	template<int N>
	struct CreateSemaphoreTrait
	{
		static Semaphore* Create()
		{
			return new Semaphore(N);
		}
	};

	template<int N>
	struct LazySemaphore
	{
		typedef typename LazyDynamicInstance<Semaphore, CreateSemaphoreTrait<N>,
				ThreadSafeInitOnceTrait>::type type;
	};

#define LAZY_SEMAPHORE_INITIALIZER LAZY_DYNAMIC_INSTANCE_INITIALIZER

	}
	}

#endif  // V8_PLATFORM_SEMAPHORE_H_
