/*
 * BasicLayout.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_BASICLAYOUT_HH
#define _LOG4CPP_BASICLAYOUT_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/Layout.hh>

namespace log4cpp {

    /**
     * BasicLayout is a simple fixed format Layout implementation. 
     **/
    class LOG4CPP_EXPORT BasicLayout : public Layout {
        public:
        BasicLayout();
        virtual ~BasicLayout();

        /**
         * Formats the LoggingEvent in BasicLayout style:<br>
         * "timeStamp priority category ndc: message"
         **/
        virtual std::string format(const LoggingEvent& event);
    };        
}

#endif // _LOG4CPP_BASICLAYOUT_HH
