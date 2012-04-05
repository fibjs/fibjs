/*
 * Properties.hh
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_PROPERTIES_HH
#define _LOG4CPP_PROPERTIES_HH

#include "PortabilityImpl.hh"
#include <string>
#include <iostream>
#include <map>

namespace log4cpp {
    
    class Properties : public std::map<std::string, std::string> {
        public:
        Properties();
        virtual ~Properties();

        virtual void load(std::istream& in);
        virtual void save(std::ostream& out);

        virtual int getInt(const std::string& property, int defaultValue);
        virtual bool getBool(const std::string& property, bool defaultValue);
        virtual std::string getString(const std::string& property,
                                      const char* defaultValue);

        protected:
        virtual void _substituteVariables(std::string& value);
    };
}

#endif // _LOG4CPP_PROPERTIES_HH

