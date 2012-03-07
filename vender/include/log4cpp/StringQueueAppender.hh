/*
 * StringQueueAppender.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_STRINGQUEUEAPPENDER_HH
#define _LOG4CPP_STRINGQUEUEAPPENDER_HH

#include <log4cpp/Portability.hh>
#include <string>
#include <queue>
#include <log4cpp/LayoutAppender.hh>

namespace log4cpp {

    /**
     * This class puts log messages in an in-memory queue. Its primary use 
     * is in test cases, but it may be useful elsewhere as well.
     *
     * @since 0.2.4
     **/
    class LOG4CPP_EXPORT StringQueueAppender : public LayoutAppender {
        public:

        StringQueueAppender(const std::string& name);
        virtual ~StringQueueAppender();
        
        virtual bool reopen();
        virtual void close();

        /**
         * Return the current size of the message queue.
         * Shorthand for getQueue().size().
         * @returns the queue size
         **/
        virtual size_t queueSize() const;

        /**
         * Return the queue to which the Appends adds messages.
         * @returns the message queue
         **/
        virtual std::queue<std::string>& getQueue();

        /**
         * Return the queue to which the Appends adds messages.
         * @returns the message queue
         **/
        virtual const std::queue<std::string>& getQueue() const;

        /**
         * Pop the oldest log message from the front of the queue.
         * @returns the oldest log message
         **/
        virtual std::string popMessage();

        protected:
        
        /**
         * Appends the LoggingEvent to the queue.
         * @param event the LoggingEvent to layout and append to the queue.
         **/
        virtual void _append(const LoggingEvent& event);

        std::queue<std::string> _queue;
    };
}

#endif // _LOG4CPP_STRINGQUEUEAPPENDER_HH
