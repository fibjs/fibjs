/*
 * MSThreads.hh
 *
 * Copyright 2002, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2002, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_THREADING_MSTHREADS_HH
#define _LOG4CPP_THREADING_MSTHREADS_HH

#include <string>

// deal with ERROR #define
// N.B. This #includes windows.h with NOGDI and WIN32_LEAN_AND_MEAN #defined.
//      If this is not what the user wants, #include windows.h before this file.
#ifndef _WINDOWS_
#  ifndef NOGDI
#    define NOGDI  // this will circumvent the ERROR #define in windows.h
#    define LOG4CPP_UNDEFINE_NOGDI
#  endif

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    define LOG4CPP_UNDEFINE_WIN32_LEAN_AND_MEAN
#  endif

#  include <windows.h>

#  ifdef LOG4CPP_UNDEFINE_NOGDI
#    undef NOGDI
#  endif

#  ifdef LOG4CPP_UNDEFINE_WIN32_LEAN_AND_MEAN
#    undef WIN32_LEAN_AND_MEAN
#  endif

#endif // done dealing with ERROR #define

namespace log4cpp {
    namespace threading {
        /**
         * Return an identifier for the current thread. What these 
         * identifiers look like is completely up to the underlying 
         * thread library.
         **/
        std::string getThreadId();
        
        /**
         * A simple object wrapper around CreateMutex() and DeleteMutex()
         */
        class LOG4CPP_EXPORT MSMutex {
            public:
            MSMutex() { InitializeCriticalSection(&_criticalSection); }
            ~MSMutex() { DeleteCriticalSection(&_criticalSection); }
            inline LPCRITICAL_SECTION getCriticalSection() {
                return &_criticalSection;
            }

            private:
            MSMutex(const MSMutex& other);
            CRITICAL_SECTION _criticalSection;
        };

        /**
         * A simple, non recursive Mutex.
         **/
        typedef MSMutex Mutex;

        /**
         * A simple object wrapper around WaitForSingleObject() and
         * ReleaseMutex()
         */
        class MSScopedLock {
            public:
            MSScopedLock(MSMutex& mutex) {
                _criticalSection = mutex.getCriticalSection();
                EnterCriticalSection(_criticalSection);
            }

            ~MSScopedLock() { LeaveCriticalSection(_criticalSection); }

            private:
            MSScopedLock(const MSScopedLock& other);
            LPCRITICAL_SECTION _criticalSection;
        };

        /**
         * A simple "resource acquisition is initialization" idiom type lock
         * for Mutex. 
         **/
        typedef MSScopedLock ScopedLock;

        /** 
         * This class holds Thread local data of type T, i.e. for each
         * thread a ThreadLocalDataHolder holds 0 or 1 instance of T. 
         * The held object must be heap allocated and will be deleted 
         * upon termination of the thread to which it belongs.
         **/
        template<typename T> class ThreadLocalDataHolder {
            public:
            inline ThreadLocalDataHolder() :
                _key(TlsAlloc()) {};

            inline ~ThreadLocalDataHolder() { TlsFree(_key); };
            
            /**
             * Obtains the Object held for the current thread.
             * @return a pointer to the held Object or NULL if no
             * Object has been set for the current thread.
             **/
            inline T* get() const {
                return (T*)TlsGetValue(_key);
            };

            /**
             * Obtains the Object held for the current thread. 
             * Initially each thread holds NULL.
             * @return a pointer to the held Object or NULL if no
             * Object has been set for the current thread.
             **/
            inline T* operator->() const { return get(); };

            /**
             * Obtains the Object held for the current thread.
             * @pre get() != NULL
             * @return a reference to the held Object.
             **/
            inline T& operator*() const { return *get(); };

            /**
             * Releases the Object held for the current thread.
             * @post get() == NULL
             * @return a pointer to the Object thas was held for 
             * the current thread or NULL if no Object was held.
             **/
            inline T* release() {
                T* result = (T*)TlsGetValue(_key);
                TlsSetValue(_key, NULL);
                return result;
            };

            /**
             * Sets a new Object to be held for the current thread. A 
             * previously set Object will be deleted.
             * @param p the new object to hold.
             * @post get() == p
             **/
            inline void reset(T* p = NULL) {
                T* thing = (T*)TlsGetValue(_key);
                delete thing;
                TlsSetValue(_key, p);
            };

            private:            
            DWORD _key;            
        };
    }
}
#endif
