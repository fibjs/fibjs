
#include "platform/semaphore.h"

namespace v8
{
namespace internal
{

Semaphore::Semaphore(int count) :
		native_handle_(count)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::Signal()
{
	native_handle_.post();
}

void Semaphore::Wait()
{
	native_handle_.wait();
}

bool Semaphore::WaitFor(const TimeDelta& rel_time)
{
	native_handle_.wait();
	return true;
}

}
}  // namespace v8::internal
