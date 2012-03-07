/*
 * Appender.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_APPENDER_HH
#define _LOG4CPP_APPENDER_HH

#include <log4cpp/Portability.hh>
#include <string>
#include <map>
#include <set>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log4cpp/Priority.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/threading/Threading.hh>

namespace log4cpp {
    class LOG4CPP_EXPORT Filter;
    
    /**
     *  Implement this interface for your own strategies for printing log
     *  statements.
     **/
    class LOG4CPP_EXPORT Appender {
        public:
        
        /**
         * Get a pointer to an exitsing Appender.
         * @param name The name of the Appender to return.
         * @returns a pointer to an existing Appender, or NULL if no appender
         * with the specfied name exists.
         **/
        static Appender* getAppender(const std::string& name);
        
        /**
         * Call reopen() on all existing Appenders.
         * @returns true if all Appenders returned true on their reopen() call.
         **/
        static bool reopenAll();
        
        /**
         * Call reopen() on all existing Appenders.
         * @returns true if all Appenders returned true on their reopen() call.
         **/
        static void closeAll();
        
        protected:
        /**
         * Constructor for Appender. Will only be used in getAppender() (and
         * in derived classes of course).
         * @param name The name of this Appender.
         **/
        Appender(const std::string& name);
        
        public:
        /**
         * Destructor for Appender.
         **/
        virtual ~Appender();
        
        /**
         * Log in Appender specific way.
         * @param event  The LoggingEvent to log.
         **/
        virtual void doAppend(const LoggingEvent& event) = 0;
        
        /**
         * Reopens the output destination of this Appender, e.g. the logfile
         * or TCP socket.
         * @returns false if an error occured during reopening, true otherwise.
         **/
        virtual bool reopen() = 0;
        
        /**
         * Release any resources allocated within the appender such as file
         * handles, network connections, etc.
         **/
        virtual void close() = 0;
        
        /**
         * Check if the appender uses a layout.
         *
         * @returns true if the appender implementation requires a layout.
         **/
        virtual bool requiresLayout() const = 0;
        
        /**
         * Set the Layout for this appender.
         * @param layout The layout to use.
         **/
        virtual void setLayout(Layout* layout) = 0;
        
        /**
         * Get the name of this appender. The name identifies the appender.
         * @returns the name of the appender.
         **/
        inline const std::string& getName() const { return _name; };
        
        /**
         * Set the threshold priority of this Appender. The Appender will not
         * appender LoggingEvents with a priority lower than the threshold.
         * Use Priority::NOTSET to disable threshold checking.
         * @param priority The priority to set.
         **/
        virtual void setThreshold(Priority::Value priority) = 0;
        
        /**
         * Get the threshold priority of this Appender.
         * @returns the threshold
         **/
        virtual Priority::Value getThreshold() = 0;
        
        /**
         * Set a Filter for this appender.
         **/
        virtual void setFilter(Filter* filter) = 0;
        
        /**
         * Get the Filter for this appender.
         * @returns the filter, or NULL if no filter has been set.
         **/
        virtual Filter* getFilter() = 0;
        
        private:
        typedef std::map<std::string, Appender*> AppenderMap;
        
        static AppenderMap* _allAppenders;
        static threading::Mutex _appenderMapMutex;

        static AppenderMap& _getAllAppenders();
        static void _deleteAllAppenders();
        static void _addAppender(Appender* appender);
        static void _removeAppender(Appender* appender);

        const std::string _name;
    };

    typedef std::set<Appender *> AppenderSet;
}

#endif // _LOG4CPP_APPENDER_HH
