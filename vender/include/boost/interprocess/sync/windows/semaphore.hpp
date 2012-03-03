//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_WINDOWS_SEMAPHORE_HPP
#define BOOST_INTERPROCESS_DETAIL_WINDOWS_SEMAPHORE_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/detail/posix_time_types_wrk.hpp>
#include <boost/interprocess/detail/win32_api.hpp>
#include <boost/interprocess/detail/intermodule_singleton.hpp>
#include <boost/interprocess/sync/windows/sync_utils.hpp>
#include <boost/interprocess/exceptions.hpp>


namespace boost {
namespace interprocess {
namespace ipcdetail {

class windows_semaphore
{
   windows_semaphore(const windows_semaphore &);
   windows_semaphore &operator=(const windows_semaphore &);
   public:

   windows_semaphore(unsigned int initialCount);
   ~windows_semaphore();

   void post(long release_count = 1);
   void wait();
   bool try_wait();
   bool timed_wait(const boost::posix_time::ptime &abs_time);

   private:
   const sync_id id_;
};

inline windows_semaphore::windows_semaphore(unsigned int initialCount) 
   : id_()
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //Force smeaphore creation with the initial count
   bool open_or_created;
   handles.obtain_semaphore(this->id_, initialCount, &open_or_created);
   //The semaphore must be created, never opened
   assert(open_or_created);
   assert(open_or_created && winapi::get_last_error() != winapi::error_already_exists);
   (void)open_or_created;
}

inline windows_semaphore::~windows_semaphore() 
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   handles.destroy_handle(this->id_);
}

inline void windows_semaphore::wait(void)
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_semaphore(this->id_, 0);
   unsigned long ret = winapi::wait_for_single_object(hnd, winapi::infinite_time);
   if(ret == winapi::wait_failed){
      error_info err(winapi::get_last_error());
      throw interprocess_exception(err);
   }
}

inline bool windows_semaphore::try_wait(void)
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_semaphore(this->id_, 0);
   unsigned long ret = winapi::wait_for_single_object(hnd, 0);
   if(ret == winapi::wait_failed){
      error_info err(winapi::get_last_error());
      throw interprocess_exception(err);
   }
   return ret != winapi::wait_timeout;
}

inline bool windows_semaphore::timed_wait(const boost::posix_time::ptime &abs_time)
{
   if(abs_time == boost::posix_time::pos_infin){
      this->wait();
      return true;
   }
   boost::posix_time::ptime now
      = boost::posix_time::microsec_clock::universal_time();

   unsigned long ms = (unsigned long)(abs_time-now).total_milliseconds();
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_semaphore(this->id_, 0);
   unsigned long ret = winapi::wait_for_single_object(hnd, ms);
   if(ret == winapi::wait_failed){
      error_info err(winapi::get_last_error());
      throw interprocess_exception(err);
   }
   return ret != winapi::wait_timeout;
}

inline void windows_semaphore::post(long release_count)
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_semaphore(this->id_, 0);
   long prev_count;
   int ret = winapi::release_semaphore(hnd, release_count, &prev_count);
   (void)ret;
   assert(ret);
}

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_WINDOWS_SEMAPHORE_HPP
