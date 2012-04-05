/*
 * PropertyConiguratorImpl.hh
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_PROPERTYCONFIGURATORIMPL_HH
#define _LOG4CPP_PROPERTYCONFIGURATORIMPL_HH

#include "PortabilityImpl.hh"
#include <log4cpp/Configurator.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>
#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "Properties.hh"

namespace log4cpp {
    
    class PropertyConfiguratorImpl {
        public:
        typedef std::map<std::string, Appender*> AppenderMap;

        PropertyConfiguratorImpl();
        virtual ~PropertyConfiguratorImpl();
        virtual void doConfigure(const std::string& initFileName)
            throw (ConfigureFailure);
        virtual void doConfigure(std::istream& in)
            throw (ConfigureFailure);

        protected:
        /**
           configure the given category. This includes setting its Priority
           and adding any Appenders. 
           @todo setting other properties like 'additivity'.
           @param categoryname	Name of the category to configure. 
           The name 'rootCategory' refers to the root Category.
           throw ConfigureFailure
         **/
        void configureCategory(const std::string& categoryname) throw (ConfigureFailure);

        /**
         * Get a list of categories for which we should do the configuration.  This simply
         * extracts the categories from the map.
         * @param categories Reference to a list which is to receive the list of categories.
         */
        void getCategories(std::vector<std::string>& categories) const;

        void instantiateAllAppenders() throw(ConfigureFailure);

        /**
         * Intantiate and configure the appender referred to by the given name. This method searches the
         * map to find all configuration parameters for the appender, and adds the appender
         * to the given category.  This isn't very general in the sense that it will need to
         * be modified for each type of appender and layout.  A more general solution would 
         * be to define an "options" interface for each appender and layout, so that we can 
         * simply call this method with a list of options instead of needing to know what is 
         * or is not available.  This would also require some generic way of instantiating an
         * object for which we have no knowledge.  An "AppenderFactory" could be used which
         * maps the given type to an actual object class registered with the factory (?? is this
         * possible?).
         * @param name	String containing the name of the type of appender to be instantiated.
         */
        Appender* instantiateAppender(const std::string& name);

        /**
         * Method for instantiating and configuring the layouts associated with each
         * appender.
         * @param appender	Appender to which we are setting this layout.
         * @param name	Name in the properties of this appender.
         */
        void setLayout(Appender* appender, const std::string& name);

        Properties _properties;
        AppenderMap _allAppenders;
    };
}

#endif // _LOG4CPP_PROPERTIES_HH

