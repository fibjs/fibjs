//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_WINDOWS_MUTEX_HPP
#define BOOST_INTERPROCESS_DETAIL_WINDOWS_MUTEX_HPP

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

class windows_mutex
{
   windows_mutex(const windows_mutex &);
   windows_mutex &operator=(const windows_mutex &);
   public:

   windows_mutex();
   ~windows_mutex();

   void lock();
   bool try_lock();
   bool timed_lock(const boost::posix_time::ptime &abs_time);
   void unlock();
   void take_ownership(){};

   private:
   const sync_id id_;
};

inline windows_mutex::windows_mutex() 
   : id_()
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //Create mutex with the initial count
   bool open_or_created;
   handles.obtain_mutex(this->id_, &open_or_created);
   //The mutex must be created, never opened
   assert(open_or_created);
   assert(open_or_created && winapi::get_last_error() != winapi::error_already_exists);
   (void)open_or_created;
}

inline windows_mutex::~windows_mutex() 
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   handles.destroy_handle(this->id_);
}

inline void windows_mutex::lock(void)
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_mutex(this->id_);
   unsigned long ret = winapi::wait_for_single_object(hnd, winapi::infinite_time);
   if(ret == winapi::wait_failed){
      error_info err(winapi::get_last_error());
      throw interprocess_exception(err);
   }
}

inline bool windows_mutex::try_lock(void)
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_mutex(this->id_);
   unsigned long ret = winapi::wait_for_single_object(hnd, 0);
   if(ret == winapi::wait_failed){
      error_info err(winapi::get_last_error());
      throw interprocess_exception(err);
   }
   return ret != winapi::wait_timeout;
}

inline bool windows_mutex::timed_lock(const boost::posix_time::ptime &abs_time)
{
   if(abs_time == boost::posix_time::pos_infin){
      this->lock();
      return true;
   }
   boost::posix_time::ptime now
      = boost::posix_time::microsec_clock::universal_time();

   unsigned long ms = (unsigned long)(abs_time-now).total_milliseconds();
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_mutex(this->id_);
   unsigned long ret = winapi::wait_for_single_object(hnd, ms);
   if(ret == winapi::wait_failed){
      error_info err(winapi::get_last_error());
      throw interprocess_exception(err);
   }
   return ret != winapi::wait_timeout;
}

inline void windows_mutex::unlock(void)
{
   sync_handles &handles =
      intermodule_singleton<sync_handles>::get();
   //This can throw
   void *hnd = handles.obtain_mutex(this->id_);
   int ret = winapi::release_mutex(hnd);
   (void)ret;
   assert(ret);
}

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_WINDOWS_MUTEX_HPP
