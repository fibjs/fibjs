/*
 * Layout.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_LAYOUT_HH
#define _LOG4CPP_LAYOUT_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/LoggingEvent.hh>
#include <string>

namespace log4cpp {

/**
 * Extend this abstract class to create your own log layout format.
 **/
    class LOG4CPP_EXPORT Layout {
        public:
        /**
         * Destructor for Layout.
         **/
        virtual ~Layout() { };

        /**
         * Formats the LoggingEvent data to a string that appenders can log.
         * Implement this method to create your own layout format.
         * @param event The LoggingEvent.
         * @returns an appendable string.
         **/
        virtual std::string format(const LoggingEvent& event) = 0;
    };        
}

#endif // _LOG4CPP_LAYOUT_HH
