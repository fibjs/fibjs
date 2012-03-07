/*
 * Configurator.hh
 *
 * Copyright 2001, Glen Scott. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */
#ifndef _LOG4CPP_CONFIGURATOR_HH
#define _LOG4CPP_CONFIGURATOR_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/Export.hh>
#include <string>
#include <stdexcept>

namespace log4cpp {

    /**
     * Exception class for configuration.
     */
    class LOG4CPP_EXPORT ConfigureFailure : public std::runtime_error {
        public:
        /**
         * Constructor.
         * @param reason String containing the description of the exception.
         */
        ConfigureFailure(const std::string& reason);
    };
}

#endif // _LOG4CPP_CONFIGURATOR_HH
