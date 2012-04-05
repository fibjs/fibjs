/*
 * Configurator.cpp
 *
 * Copyright 2001, Glen Scott. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */
#include "PortabilityImpl.hh"
#include <log4cpp/Configurator.hh>

namespace log4cpp {

    ConfigureFailure::ConfigureFailure(const std::string& reason) :
        std::runtime_error(reason) {
    }
}




