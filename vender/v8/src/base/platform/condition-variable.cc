
#include "condition-variable.h"

namespace v8 {
namespace base {

ConditionVariable::ConditionVariable() {
}


ConditionVariable::~ConditionVariable() {
}


void ConditionVariable::NotifyOne() {
  native_handle_.notify_one();
}


void ConditionVariable::NotifyAll() {
  native_handle_.notify_all();
}


void ConditionVariable::Wait(Mutex* mutex) {
  native_handle_.wait(mutex->native_handle());
}


bool ConditionVariable::WaitFor(Mutex* mutex, const TimeDelta& rel_time) {
  native_handle_.wait(mutex->native_handle());
  return true;
}

} }  // namespace v8::base
