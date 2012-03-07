/*
 * AbortAppender.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/AbortAppender.hh>
#include <log4cpp/FactoryParams.hh>
#include <cstdlib>
#include <memory>

namespace log4cpp {

    AbortAppender::AbortAppender(const std::string& name) : 
        AppenderSkeleton(name) {
    }
    
    AbortAppender::~AbortAppender() {
        close();
    }

    void AbortAppender::close() {
        // empty
    }

    void AbortAppender::_append(const LoggingEvent& event) {
        std::abort();
    }

    bool AbortAppender::reopen() {
        return true;
    }
      
    bool AbortAppender::requiresLayout() const {
        return false;
    }

    void AbortAppender::setLayout(Layout* layout) {
        return;
    }

    std::auto_ptr<Appender> create_abort_appender(const FactoryParams& params)
    {
       std::string name;
       params.get_for("abort appender").required("name", name);
       return std::auto_ptr<Appender>(new AbortAppender(name));
    }
}
