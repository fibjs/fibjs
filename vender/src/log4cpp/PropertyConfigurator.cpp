/*
 * PropertyConfigurator.cpp
 *
 * Copyright 2001, Glen Scott. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */
#include "PortabilityImpl.hh"
#include <log4cpp/PropertyConfigurator.hh>
#include "PropertyConfiguratorImpl.hh"

namespace log4cpp {

    void PropertyConfigurator::configure(const std::string& initFileName) throw (ConfigureFailure) {
        PropertyConfiguratorImpl configurator;
        
        configurator.doConfigure(initFileName);
    }
}

