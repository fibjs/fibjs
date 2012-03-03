//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_WINDOWS_CONDITION_HPP
#define BOOST_INTERPROCESS_DETAIL_WINDOWS_CONDITION_HPP

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/detail/posix_time_types_wrk.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/sync/windows/semaphore.hpp>
#include <boost/interprocess/sync/windows/mutex.hpp>
#include <boost/cstdint.hpp>
#include <limits>

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// 
// Condition variable algorithm taken from pthreads-win32 discussion.
//
// The algorithm was developed by Alexander Terekhov in colaboration with
// Louis Thomas.
// 
//     Algorithm 8a / IMPL_SEM,UNBLOCK_STRATEGY == UNBLOCK_ALL
//
// semBlockLock - bin.semaphore
// semBlockQueue - semaphore
// mtxExternal - mutex or CS
// mtxUnblockLock - mutex or CS
// nWaitersGone - int
// nWaitersBlocked - int
// nWaitersToUnblock - int
// 
// wait( timeout ) {
// 
//   [auto: register int result          ]     // error checking omitted
//   [auto: register int nSignalsWasLeft ]
//   [auto: register int nWaitersWasGone ]
// 
//   sem_wait( semBlockLock );
//   nWaitersBlocked++;
//   sem_post( semBlockLock );
// 
//   unlock( mtxExternal );
//   bTimedOut = sem_wait( semBlockQueue,timeout );
// 
//   lock( mtxUnblockLock );
//   if ( 0 != (nSignalsWasLeft = nWaitersToUnblock) ) {
//     if ( bTimedOut ) {                       // timeout (or canceled)
//       if ( 0 != nWaitersBlocked ) {
//         nWaitersBlocked--;
//       }
//       else {
//         nWaitersGone++;                     // count spurious wakeups.
//       }
//     }
//     if ( 0 == --nWaitersToUnblock ) {
//       if ( 0 != nWaitersBlocked ) {
//         sem_post( semBlockLock );           // open the gate.
//         nSignalsWasLeft = 0;                // do not open the gate
//                                             // below again.
//       }
//       else if ( 0 != (nWaitersWasGone = nWaitersGone) ) {
//         nWaitersGone = 0;
//       }
//     }
//   }
//   else if ( INT_MAX/2 == ++nWaitersGone ) { // timeout/canceled or
//                                             // spurious semaphore :-)
//     sem_wait( semBlockLock );
//     nWaitersBlocked -= nWaitersGone;     // something is going on here
//                                          //  - test of timeouts? :-)
//     sem_post( semBlockLock );
//     nWaitersGone = 0;
//   }
//   unlock( mtxUnblockLock );
// 
//   if ( 1 == nSignalsWasLeft ) {
//     if ( 0 != nWaitersWasGone ) {
//       // sem_adjust( semBlockQueue,-nWaitersWasGone );
//       while ( nWaitersWasGone-- ) {
//         sem_wait( semBlockQueue );       // better now than spurious later
//       }
//     } sem_post( semBlockLock );          // open the gate
//   }
// 
//   lock( mtxExternal );
// 
//   return ( bTimedOut ) ? ETIMEOUT : 0;
// }
// 
// signal(bAll) {
// 
//   [auto: register int result         ]
//   [auto: register int nSignalsToIssue]
// 
//   lock( mtxUnblockLock );
// 
//   if ( 0 != nWaitersToUnblock ) {        // the gate is closed!!!
//     if ( 0 == nWaitersBlocked ) {        // NO-OP
//       return unlock( mtxUnblockLock );
//     }
//     if (bAll) {
//       nWaitersToUnblock += nSignalsToIssue=nWaitersBlocked;
//       nWaitersBlocked = 0;
//     }
//     else {
//       nSignalsToIssue = 1;
//       nWaitersToUnblock++;
//       nWaitersBlocked--;
//     }
//   }
//   else if ( nWaitersBlocked > nWaitersGone ) { // HARMLESS RACE CONDITION!
//     sem_wait( semBlockLock );                  // close the gate
//     if ( 0 != nWaitersGone ) {
//       nWaitersBlocked -= nWaitersGone;
//       nWaitersGone = 0;
//     }
//     if (bAll) {
//       nSignalsToIssue = nWaitersToUnblock = nWaitersBlocked;
//       nWaitersBlocked = 0;
//     }
//     else {
//       nSignalsToIssue = nWaitersToUnblock = 1;
//       nWaitersBlocked--;
//     }
//   }
//   else { // NO-OP
//     return unlock( mtxUnblockLock );
//   }
// 
//   unlock( mtxUnblockLock );
//   sem_post( semBlockQueue,nSignalsToIssue );
//   return result;
// }
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

namespace boost {
namespace interprocess {
namespace ipcdetail {

class windows_condition
{
   windows_condition(const windows_condition &);
   windows_condition &operator=(const windows_condition &);
   public:
   windows_condition();
   ~windows_condition();

   void notify_one();
   void notify_all();

   template <typename L>
   bool timed_wait(L& lock, const boost::posix_time::ptime &abs_time)
   {
      if(abs_time == boost::posix_time::pos_infin){
         this->wait(lock);
         return true;
      }
      if (!lock)
         throw lock_exception();
      return this->do_timed_wait(abs_time, *lock.mutex());
   }

   template <typename L, typename Pr>
   bool timed_wait(L& lock, const boost::posix_time::ptime &abs_time, Pr pred)
   {
      if(abs_time == boost::posix_time::pos_infin){
         this->wait(lock, pred);
         return true;
      }
      if (!lock)
         throw lock_exception();
      while (!pred()){
         if (!this->do_timed_wait(abs_time, *lock.mutex()))
            return pred();
      }
      return true;
   }

   template <typename L>
   void wait(L& lock)
   {
      if (!lock)
         throw lock_exception();
      do_wait(*lock.mutex());
   }

   template <typename L, typename Pr>
   void wait(L& lock, Pr pred)
   {
      if (!lock)
         throw lock_exception();

      while (!pred())
         do_wait(*lock.mutex());
   }

   template<class InterprocessMutex>
   void do_wait(InterprocessMutex &mut);

   template<class InterprocessMutex>
   bool do_timed_wait(const boost::posix_time::ptime &abs_time, InterprocessMutex &mut);

   private:

   template<class InterprocessMutex>
   bool do_timed_wait(bool timeout_enabled, const boost::posix_time::ptime &abs_time, InterprocessMutex &mut);
   void do_signal    (bool broadcast);

   boost::int32_t    m_nwaiters_blocked;
   boost::int32_t    m_nwaiters_gone;
   boost::int32_t    m_nwaiters_to_unblock;
   windows_semaphore m_sem_block_queue;
   windows_semaphore m_sem_block_lock;
   windows_mutex     m_mtx_unblock_lock;
};

inline windows_condition::windows_condition()
 : m_nwaiters_blocked(0)
 , m_nwaiters_gone(0)
 , m_nwaiters_to_unblock(0)
 , m_sem_block_queue(0)
 , m_sem_block_lock(1)
 , m_mtx_unblock_lock()
{}

inline windows_condition::~windows_condition()
{}

inline void windows_condition::notify_one()
{  this->do_signal(false);  }

inline void windows_condition::notify_all()
{  this->do_signal(true);  }

inline void windows_condition::do_signal(bool broadcast)
{
   boost::int32_t nsignals_to_issue;

   {
      scoped_lock<windows_mutex> locker(m_mtx_unblock_lock);

      if ( 0 != m_nwaiters_to_unblock ) {        // the gate is closed!!!
         if ( 0 == m_nwaiters_blocked ) {        // NO-OP
            //locker's destructor triggers m_mtx_unblock_lock.unlock() 
            return;
         }
         if (broadcast) {
            m_nwaiters_to_unblock += nsignals_to_issue = m_nwaiters_blocked;
            m_nwaiters_blocked = 0;
         }
         else {
            nsignals_to_issue = 1;
            m_nwaiters_to_unblock++;
            m_nwaiters_blocked--;
         }
      }
      else if ( m_nwaiters_blocked > m_nwaiters_gone ) { // HARMLESS RACE CONDITION!
         m_sem_block_lock.wait();                      // close the gate
         if ( 0 != m_nwaiters_gone ) {
            m_nwaiters_blocked -= m_nwaiters_gone;
            m_nwaiters_gone = 0;
         }
         if (broadcast) {
            nsignals_to_issue = m_nwaiters_to_unblock = m_nwaiters_blocked;
            m_nwaiters_blocked = 0;
         }
         else {
            nsignals_to_issue = m_nwaiters_to_unblock = 1;
            m_nwaiters_blocked--;
         }
      }
      else { // NO-OP
         //locker's destructor triggers m_mtx_unblock_lock.unlock() 
         return;
      }
      //locker's destructor triggers m_mtx_unblock_lock.unlock() 
   }
   m_sem_block_queue.post(nsignals_to_issue);
}

template<class InterprocessMutex>
inline void windows_condition::do_wait(InterprocessMutex &mut)
{  this->do_timed_wait(false, boost::posix_time::ptime(), mut);  }

template<class InterprocessMutex>
inline bool windows_condition::do_timed_wait
   (const boost::posix_time::ptime &abs_time, InterprocessMutex &mut)
{  return this->do_timed_wait(true, abs_time, mut);  }

template<class InterprocessMutex>
inline bool windows_condition::do_timed_wait
   (bool tout_enabled, const boost::posix_time::ptime &abs_time, InterprocessMutex &mtxExternal)
{
   //Initialize to avoid warnings
   boost::int32_t nsignals_was_left = 0;
   boost::int32_t nwaiters_was_gone = 0;
 
   m_sem_block_lock.wait();
   ++m_nwaiters_blocked;
   m_sem_block_lock.post();

   struct scoped_unlock
   {
      InterprocessMutex & mut;
      scoped_unlock(InterprocessMutex & m)
         : mut(m)
      {  m.unlock();  }

      ~scoped_unlock()
      {  mut.lock();  }
   } unlocker(mtxExternal);
 

   bool bTimedOut = tout_enabled ? !m_sem_block_queue.timed_wait(abs_time) : (m_sem_block_queue.wait(), false);
 
   {
      scoped_lock<windows_mutex> locker(m_mtx_unblock_lock);
      if ( 0 != (nsignals_was_left = m_nwaiters_to_unblock) ) {
         if ( bTimedOut ) {                       // timeout (or canceled)
            if ( 0 != m_nwaiters_blocked ) {
               m_nwaiters_blocked--;
            }
            else {
               m_nwaiters_gone++;                     // count spurious wakeups.
            }
         }
         if ( 0 == --m_nwaiters_to_unblock ) {
            if ( 0 != m_nwaiters_blocked ) {
               m_sem_block_lock.post();          // open the gate.
               nsignals_was_left = 0;          // do not open the gate below again.
            }
            else if ( 0 != (nwaiters_was_gone = m_nwaiters_gone) ) {
               m_nwaiters_gone = 0;
            }
         }
      }
      else if ( (std::numeric_limits<boost::int32_t>::max)()/2
                == ++m_nwaiters_gone ) { // timeout/canceled or spurious semaphore :-)
         m_sem_block_lock.wait();
         m_nwaiters_blocked -= m_nwaiters_gone;       // something is going on here - test of timeouts? :-)
         m_sem_block_lock.post();
         m_nwaiters_gone = 0;
      }
      //locker's destructor triggers m_mtx_unblock_lock.unlock() 
   }  
 
   if ( 1 == nsignals_was_left ) {
      if ( 0 != nwaiters_was_gone ) {
         // sem_adjust( m_sem_block_queue,-nwaiters_was_gone );
         while ( nwaiters_was_gone-- ) {
            m_sem_block_queue.wait();       // better now than spurious later
         }
      }
      m_sem_block_lock.post(); // open the gate
   }
 
   //mtxExternal.lock(); called from unlocker
 
   return ( bTimedOut ) ? false : true;
}

}  //namespace ipcdetail
}  //namespace interprocess
}  //namespace boost

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_WINDOWS_CONDITION_HPP
