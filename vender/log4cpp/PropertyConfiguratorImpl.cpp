/*
 * PropertyConfiguratorImpl.cpp
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */
#include "PortabilityImpl.hh"

#ifdef LOG4CPP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef LOG4CPP_HAVE_IO_H
#    include <io.h>
#endif
#include <iostream>

#include <string>
#include <fstream>

#include <log4cpp/Category.hh>

// appenders
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/AbortAppender.hh>
#ifdef WIN32
#include <log4cpp/Win32DebugAppender.hh>
#include <log4cpp/NTEventLogAppender.hh>
#endif
#include <log4cpp/RemoteSyslogAppender.hh>
#ifdef LOG4CPP_HAVE_LIBIDSA
#include <log4cpp/IdsaAppender.hh>
#endif	// LOG4CPP_HAVE_LIBIDSA
#ifdef LOG4CPP_HAVE_SYSLOG
#include <log4cpp/SyslogAppender.hh>
#endif

// layouts
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/PatternLayout.hh>

#include <log4cpp/Priority.hh>
#include <log4cpp/NDC.hh>

#include <list>
#include <vector>
#include <iterator>

#include "PropertyConfiguratorImpl.hh"
#include "StringUtil.hh"

namespace log4cpp {

    PropertyConfiguratorImpl::PropertyConfiguratorImpl() {
    }

    PropertyConfiguratorImpl::~PropertyConfiguratorImpl() {
    }

    void PropertyConfiguratorImpl::doConfigure(const std::string& initFileName) throw (ConfigureFailure) {
        std::ifstream initFile(initFileName.c_str());

        if (!initFile) {
            throw ConfigureFailure(std::string("File ") + initFileName + " does not exist");
        }

        doConfigure(initFile);
    }


    void PropertyConfiguratorImpl::doConfigure(std::istream& in) throw (ConfigureFailure) {
        // parse the file to get all of the configuration
        _properties.load(in);

        instantiateAllAppenders();
        // get categories
        std::vector<std::string> catList;
        getCategories(catList);

        // configure each category
        for(std::vector<std::string>::const_iterator iter = catList.begin();
            iter != catList.end(); ++iter) {
            configureCategory(*iter);
        }
    }

    void PropertyConfiguratorImpl::instantiateAllAppenders() throw(ConfigureFailure) {
        std::string currentAppender;

        std::string prefix("appender");
        Properties::const_iterator from = _properties.lower_bound(prefix + '.');
        Properties::const_iterator to = _properties.lower_bound(prefix + '/');
        for(Properties::const_iterator i = from; i != to; ++i) {
            const std::string& key = (*i).first;
            const std::string& value = (*i).second;
            std::list<std::string> propNameParts;
            std::back_insert_iterator<std::list<std::string> > pnpIt(propNameParts);
            StringUtil::split(pnpIt, key, '.');
            std::list<std::string>::const_iterator i2 = propNameParts.begin();
            std::list<std::string>::const_iterator iEnd = propNameParts.end();
            if (++i2 == iEnd) {
                throw ConfigureFailure(std::string("missing appender name"));
            }

            const std::string appenderName = *i2++;

            /* WARNING, approaching lame code section:
               skipping of the Appenders properties only to get them 
               again in instantiateAppender.
            */
            if (appenderName == currentAppender) {
                // simply skip properties for the current appender
            } else {
                if (i2 == iEnd) {
                    // a new appender
                    currentAppender = appenderName;
                    _allAppenders[currentAppender] = 
                        instantiateAppender(currentAppender);
                } else {
                    throw ConfigureFailure(std::string("partial appender definition : ") + key);
                }
            }                            
        }
    }

    void PropertyConfiguratorImpl::configureCategory(const std::string& categoryName) throw (ConfigureFailure) {
        // start by reading the "rootCategory" key
        std::string tempCatName = 
            (categoryName == "rootCategory") ? categoryName : "category." + categoryName;

        Properties::iterator iter = _properties.find(tempCatName);

        if (iter == _properties.end())
            throw ConfigureFailure(std::string("Unable to find category: ") + tempCatName);

        // need to get the root instance of the category
        Category& category = (categoryName == "rootCategory") ?
            Category::getRoot() : Category::getInstance(categoryName);

        
        std::list<std::string> tokens;
        std::back_insert_iterator<std::list<std::string> > tokIt(tokens);
        StringUtil::split(tokIt, (*iter).second, ',');
        std::list<std::string>::const_iterator i = tokens.begin();
        std::list<std::string>::const_iterator iEnd = tokens.end();

        Priority::Value priority = Priority::NOTSET;
        if (i != iEnd) {
            std::string priorityName = StringUtil::trim(*i++);
            try {
                if (priorityName != "") {
                    priority = Priority::getPriorityValue(priorityName);
                }
            } catch(std::invalid_argument& e) {
                throw ConfigureFailure(std::string(e.what()) + 
                    " for category '" + categoryName + "'");
            }
        }

        category.setPriority(priority);

        bool additive = _properties.getBool("additivity." + categoryName, true);
        category.setAdditivity(additive);

        category.removeAllAppenders();
        for(/**/; i != iEnd; ++i) {           
            std::string appenderName = StringUtil::trim(*i);
            AppenderMap::const_iterator appIt = 
                _allAppenders.find(appenderName);
            if (appIt == _allAppenders.end()) {
                // appender not found;
                throw ConfigureFailure(std::string("Appender '") +
                    appenderName + "' not found for category '" + categoryName + "'");
            } else {
                /* pass by reference, i.e. don't transfer ownership
                 */
                category.addAppender(*((*appIt).second));
            }
        }
    }

    Appender* PropertyConfiguratorImpl::instantiateAppender(const std::string& appenderName) {
        Appender* appender = NULL;
        std::string appenderPrefix = std::string("appender.") + appenderName;

        // determine the type by the appenderName 
        Properties::iterator key = _properties.find(appenderPrefix);
        if (key == _properties.end())
            throw ConfigureFailure(std::string("Appender '") + appenderName + "' not defined");
		
        std::string::size_type length = (*key).second.find_last_of(".");
        std::string appenderType = (length == std::string::npos) ?
            (*key).second : (*key).second.substr(length+1);

        // and instantiate the appropriate object
        if (appenderType == "ConsoleAppender") {
            appender = new OstreamAppender(appenderName, &std::cout);
        }
        else if (appenderType == "FileAppender") {
            std::string fileName = _properties.getString(appenderPrefix + ".fileName", "foobar");
            bool append = _properties.getBool(appenderPrefix + ".append", true);
            appender = new FileAppender(appenderName, fileName, append);
        }
        else if (appenderType == "RollingFileAppender") {
            std::string fileName = _properties.getString(appenderPrefix + ".fileName", "foobar");
            size_t maxFileSize = _properties.getInt(appenderPrefix + ".maxFileSize", 10*1024*1024);
            int maxBackupIndex = _properties.getInt(appenderPrefix + ".maxBackupIndex", 1);
            bool append = _properties.getBool(appenderPrefix + ".append", true);
            appender = new RollingFileAppender(appenderName, fileName, maxFileSize, maxBackupIndex,
                append);
        }
        else if (appenderType == "SyslogAppender") {
            std::string syslogName = _properties.getString(appenderPrefix + ".syslogName", "syslog");
            std::string syslogHost = _properties.getString(appenderPrefix + ".syslogHost", "localhost");
            int facility = _properties.getInt(appenderPrefix + ".facility", -1) * 8; // * 8 to get LOG_KERN, etc. compatible values. 
            int portNumber = _properties.getInt(appenderPrefix + ".portNumber", -1);
            appender = new RemoteSyslogAppender(appenderName, syslogName, 
                                                syslogHost, facility, portNumber);
        }
#ifdef LOG4CPP_HAVE_SYSLOG
        else if (appenderType == "LocalSyslogAppender") {
            std::string syslogName = _properties.getString(appenderPrefix + ".syslogName", "syslog");
            int facility = _properties.getInt(appenderPrefix + ".facility", -1) * 8; // * 8 to get LOG_KERN, etc. compatible values. 
            appender = new SyslogAppender(appenderName, syslogName, facility);
        }
#endif // LOG4CPP_HAVE_SYSLOG
        else if (appenderType == "AbortAppender") {
            appender = new AbortAppender(appenderName);
        }
#ifdef LOG4CPP_HAVE_LIBIDSA
        else if (appenderType == "IdsaAppender") {
            // default idsa name ???
            std::string idsaName = _properties.getString(appenderPrefix + ".idsaName", "foobar");

            appender = new IdsaAppender(appenderName, idsaname);
        }
#endif	// LOG4CPP_HAVE_LIBIDSA

#ifdef WIN32
        // win32 debug appender
        else if (appenderType == "Win32DebugAppender") {
            appender = new Win32DebugAppender(appenderName);
        }
        // win32 NT event log appender
        else if (appenderType == "NTEventLogAppender") {
            std::string source = _properties.getString(appenderPrefix + ".source", "foobar");
            appender = new NTEventLogAppender(appenderName, source);
        }
#endif	// WIN32
        else {
            throw ConfigureFailure(std::string("Appender '") + appenderName + 
                                   "' has unknown type '" + appenderType + "'");
        }

        if (appender->requiresLayout()) {
            setLayout(appender, appenderName);
        }

        // set threshold
        std::string thresholdName = _properties.getString(appenderPrefix + ".threshold", "");
        try {
            if (thresholdName != "") {
                appender->setThreshold(Priority::getPriorityValue(thresholdName));
            }
        } catch(std::invalid_argument& e) {
            throw ConfigureFailure(std::string(e.what()) + 
                " for threshold of appender '" + appenderName + "'");
        }

        return appender;
    }

    void PropertyConfiguratorImpl::setLayout(Appender* appender, const std::string& appenderName) {
        // determine the type by appenderName
        std::string tempString;
        Properties::iterator key = 
            _properties.find(std::string("appender.") + appenderName + ".layout");

        if (key == _properties.end())
            throw ConfigureFailure(std::string("Missing layout property for appender '") + 
                                   appenderName + "'");
		
        std::string::size_type length = (*key).second.find_last_of(".");
        std::string layoutType = (length == std::string::npos) ? 
            (*key).second : (*key).second.substr(length+1);
 
        Layout* layout;
        // and instantiate the appropriate object
        if (layoutType == "BasicLayout") {
            layout = new BasicLayout();
        }
        else if (layoutType == "SimpleLayout") {
            layout = new SimpleLayout();
        }
        else if (layoutType == "PatternLayout") {
            // need to read the properties to configure this one
            PatternLayout* patternLayout = new PatternLayout();

            key = _properties.find(std::string("appender.") + appenderName + ".layout.ConversionPattern");
            if (key == _properties.end()) {
                // leave default pattern
            } else {
                // set pattern
                patternLayout->setConversionPattern((*key).second);
            }

            layout = patternLayout;
        }
        else {
            throw ConfigureFailure(std::string("Unknown layout type '" + layoutType +
                                               "' for appender '") + appenderName + "'");
        }

        appender->setLayout(layout);
    }

    /**
     * Get the categories contained within the map of properties.  Since
     * the category looks something like "category.xxxxx.yyy.zzz", we need
     * to search the entire map to figure out which properties are category
     * listings.  Seems like there might be a more elegant solution.
     */
    void PropertyConfiguratorImpl::getCategories(std::vector<std::string>& categories) const {
        categories.clear();

        // add the root category first
        categories.push_back(std::string("rootCategory"));

        // then look for "category."
        std::string prefix("category");
        Properties::const_iterator from = _properties.lower_bound(prefix + '.');
        Properties::const_iterator to = _properties.lower_bound(prefix + (char)('.' + 1)); 
        for (Properties::const_iterator iter = from; iter != to; iter++) {
            categories.push_back((*iter).first.substr(prefix.size() + 1));
        }
    }
}
