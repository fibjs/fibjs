/*
 * LoggingEvent.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/threading/Threading.hh>

namespace log4cpp {
    
    LoggingEvent::LoggingEvent(const std::string& categoryName, 
                               const std::string& message,
                               const std::string& ndc, 
                               Priority::Value priority) :
        categoryName(categoryName),
        message(message),
        ndc(ndc),
        priority(priority),
        threadName(threading::getThreadId()) {
    }
}
