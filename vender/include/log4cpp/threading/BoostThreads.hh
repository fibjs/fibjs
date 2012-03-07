/*
 * BoostThreads.hh
 *
 * Copyright 2002, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2002, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_THREADING_BOOSTTHREADS_HH
#define _LOG4CPP_THREADING_BOOSTTHREADS_HH

#include <log4cpp/Portability.hh>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <cstdio>
#include <string>

namespace log4cpp {
    namespace threading {
        static std::string getThreadId() {
            char buffer[14];
            // Boost.Threads stores the thread ID but doesn't expose it
            sprintf(buffer, "not available");
            return std::string(buffer);
        };
        
        typedef boost::mutex Mutex;
        typedef boost::mutex::scoped_lock ScopedLock;

        template<typename T> class ThreadLocalDataHolder {
            public:
            inline T* get() const {
                return _localData.get();
            };

            inline T* operator->() const { return _localData.get(); };
            inline T& operator*() const { return *_localData.get(); };

            inline T* release() {
                return _localData.release();
            };

            inline void reset(T* p = NULL) {
                _localData.reset(p);
            };

            private:
            boost::thread_specific_ptr<T> _localData;
        };

    }
}
#endif
