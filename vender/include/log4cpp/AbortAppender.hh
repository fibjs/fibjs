/*
 * AbortAppender.hh
 *
 * Copyright 2002, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2002, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_ABORTAPPENDER_HH
#define _LOG4CPP_ABORTAPPENDER_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/AppenderSkeleton.hh>

namespace log4cpp {

    /**
     * This Appender causes the application to abort() upon the first append()
     * call. 
     *
     * @since 0.3.5
     **/
    class LOG4CPP_EXPORT AbortAppender : public AppenderSkeleton {
        public:

        AbortAppender(const std::string& name);
        virtual ~AbortAppender();
        
        virtual bool reopen();
        virtual void close();

        /**
         * The AbortAppender does not layout.
         * @returns false
         **/
        virtual bool requiresLayout() const;

        virtual void setLayout(Layout* layout);

        protected:        
        virtual void _append(const LoggingEvent& event);
    };
}

#endif // _LOG4CPP_ABORTAPPENDER_HH
