/*
 * OstreamAppender.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_OSTREAMAPPENDER_HH
#define _LOG4CPP_OSTREAMAPPENDER_HH

#include <log4cpp/Portability.hh>
#include <string>
#include <iostream>
#include <log4cpp/LayoutAppender.hh>

namespace log4cpp {

    /**
     * OstreamAppender appends LoggingEvents to ostreams.
     **/
    class LOG4CPP_EXPORT OstreamAppender : public LayoutAppender {
        public:
        OstreamAppender(const std::string& name, std::ostream* stream);
        virtual ~OstreamAppender();
        
        virtual bool reopen();
        virtual void close();

        protected:
        virtual void _append(const LoggingEvent& event);

        std::ostream* _stream;
    };
}

#endif // _LOG4CPP_OSTREAMAPPENDER_HH
