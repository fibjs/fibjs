
#ifndef V8_PLATFORM_MUTEX_H_
#define V8_PLATFORM_MUTEX_H_

#include "../lazy-instance.h"
#include <exlib/include/event.h>
#include "src/checks.h"

#if V8_OS_WIN
#include "../win32-headers.h"
#endif

namespace v8 {
namespace base {

class Mutex V8_FINAL {
 public:
  Mutex();
  ~Mutex();

  void Lock();
  void Unlock();
  bool TryLock() V8_WARN_UNUSED_RESULT;

  typedef exlib::Locker NativeHandle;

  NativeHandle& native_handle() {
    return native_handle_;
  }
  const NativeHandle& native_handle() const {
    return native_handle_;
  }

 private:
  NativeHandle native_handle_;

  V8_INLINE void AssertHeldAndUnmark() {
  }

  V8_INLINE void AssertUnheldAndMark() {
  }

  friend class ConditionVariable;

  DISALLOW_COPY_AND_ASSIGN(Mutex);
};


typedef base::LazyStaticInstance<Mutex,
                           base::DefaultConstructTrait<Mutex>,
                           base::ThreadSafeInitOnceTrait>::type LazyMutex;

#define LAZY_MUTEX_INITIALIZER LAZY_STATIC_INSTANCE_INITIALIZER


class RecursiveMutex V8_FINAL {
 public:
  RecursiveMutex();
  ~RecursiveMutex();

  void Lock();
  void Unlock();
  bool TryLock() V8_WARN_UNUSED_RESULT;
  typedef Mutex::NativeHandle NativeHandle;

  NativeHandle& native_handle() {
    return native_handle_;
  }
  const NativeHandle& native_handle() const {
    return native_handle_;
  }

 private:
  NativeHandle native_handle_;

  DISALLOW_COPY_AND_ASSIGN(RecursiveMutex);
};


typedef base::LazyStaticInstance<RecursiveMutex,
                           base::DefaultConstructTrait<RecursiveMutex>,
                           base::ThreadSafeInitOnceTrait>::type LazyRecursiveMutex;

#define LAZY_RECURSIVE_MUTEX_INITIALIZER LAZY_STATIC_INSTANCE_INITIALIZER


template <typename Mutex>
class LockGuard V8_FINAL {
 public:
  explicit LockGuard(Mutex* mutex) : mutex_(mutex) { mutex_->Lock(); }
  ~LockGuard() { mutex_->Unlock(); }

 private:
  Mutex* mutex_;

  DISALLOW_COPY_AND_ASSIGN(LockGuard);
};

} }  // namespace v8::base

#endif  // V8_PLATFORM_MUTEX_H_
