
#include "platform/mutex.h"

namespace v8 {
namespace internal {

Mutex::Mutex() {
}


Mutex::~Mutex() {
}


void Mutex::Lock() {
  native_handle_.lock();
}


void Mutex::Unlock() {
  native_handle_.unlock();
}


bool Mutex::TryLock() {
  return native_handle_.trylock();
}


RecursiveMutex::RecursiveMutex() {
}


RecursiveMutex::~RecursiveMutex() {
}


void RecursiveMutex::Lock() {
  native_handle_.lock();
}


void RecursiveMutex::Unlock() {
  native_handle_.unlock();
}


bool RecursiveMutex::TryLock() {
  return native_handle_.trylock();
}

} }  // namespace v8::internal
