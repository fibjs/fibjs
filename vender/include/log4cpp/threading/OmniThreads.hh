/*
 * OmniThreads.hh
 *
 * Copyright 2002, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2002, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_THREADING_OMNITHREADS_HH
#define _LOG4CPP_THREADING_OMNITHREADS_HH

#include <log4cpp/Portability.hh>
#include <omnithread.h>
#include <stdio.h>
#include <string>

namespace log4cpp {
    namespace threading {
        /**
         * Return an identifier for the current thread. What these 
         * identifiers look like is completely up to the underlying 
         * thread library. OmniThreads returns the POSIX thread Id.
         **/
        std::string getThreadId();
        
        /**
         * A simple, non recursive Mutex.
         * Equivalent to Boost.Threads boost::mutex
         **/
        typedef omni_mutex Mutex;

        /**
         * A simple "resource acquisition is initialization" idiom type lock
         * for Mutex. 
         * Equivalent to Boost.Threads boost::scoped_lock.
         **/
        typedef omni_mutex_lock ScopedLock;

        /** 
         * This class holds Thread local data of type T, i.e. for each
         * thread a ThreadLocalDataHolder holds 0 or 1 instance of T. 
         * The held object must be heap allocated and will be deleted 
         * upon termination of the thread to wich it belongs.
         * This is an omni_threads based equivalent of Boost.Threads 
         * thread_specific_ptr<T> class.
         **/
        template<typename T> class ThreadLocalDataHolder {
            public:
            typedef T data_type;

            inline ThreadLocalDataHolder() :
                _key(omni_thread::allocate_key()) {};

            inline ~ThreadLocalDataHolder() {};
            
            /**
             * Obtains the Object held for the current thread.
             * @return a pointer to the held Object or NULL if no
             * Object has been set for the current thread.
             **/
            inline T* get() const {
                Holder* holder = dynamic_cast<Holder*>(
                    ::omni_thread::self()->get_value(_key));
                return (holder) ? holder->data : NULL;
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
                T* result = NULL;
                Holder* holder = dynamic_cast<Holder*>(
                    ::omni_thread::self()->get_value(_key));
              
                if (holder) {
                    result = holder->data;
                    holder->data = NULL;
                }

                return result;
            };

            /**
             * Sets a new Object to be held for the current thread. A 
             * previously set Object will be deleted.
             * @param p the new object to hold.
             * @post get() == p
             **/
            inline void reset(T* p = NULL) {
                Holder* holder = dynamic_cast<Holder*>(
                    ::omni_thread::self()->get_value(_key));
                if (holder) {
                    if (holder->data)
                        delete holder->data;

                    holder->data = p;
                } else {
                    holder = new Holder(p);
                    ::omni_thread::self()->set_value(_key, holder);
                }
            };

            private:            
            class Holder : public omni_thread::value_t {
                public:
                Holder(data_type* data) : data(data) {};
                virtual ~Holder() { if (data) delete (data); };
                data_type* data;
                private:
                Holder(const Holder& other);
                Holder& operator=(const Holder& other);
            };

            omni_thread::key_t _key;            
        };
    }
}
#endif
