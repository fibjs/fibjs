/*
 * Category.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_CATEGORY_HH
#define _LOG4CPP_CATEGORY_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/threading/Threading.hh>
#include <log4cpp/convenience.h>

#include <map>
#include <vector>
#include <cstdarg>
#include <stdexcept>

namespace log4cpp {

    /**
     * This is the central class in the log4j package. One of the distintive
     * features of log4j (and hence log4cpp) are hierarchal categories and 
     * their evaluation.
     **/   
    class LOG4CPP_EXPORT Category {
        friend class HierarchyMaintainer;

        public:
        /**
         * Return the root of the Category hierarchy.
         * 
         * <p>The root category is always instantiated and available. It's
         * name is the empty string.
           
         * <p>Unlike in log4j, calling <code>Category.getInstance("")</code>
         * <em>does</em> retrieve the root category 
         * and not a category just under root named "".
         * @returns The root category
         **/
        static Category& getRoot();

        /** 
         * Set the priority of the root Category.
         * @param priority The new priority for the root Category 
         **/
        static void setRootPriority(Priority::Value priority);

        /** 
         * Get the priority of the <code>root</code> Category.
         * @returns the priority of the root category
         **/
        static Priority::Value getRootPriority() throw();

        /**
         * Instantiate a Category with name <code>name</code>. This
         * method does not set priority of the category which is by
         * default <code>Priority::NOTSET</code>.
         * 
         * @param name The name of the category to retrieve.
         **/
        static Category& getInstance(const std::string& name);
            
        /**
         * If the named category exists (in the default hierarchy) then it
         * returns a reference to the category, otherwise it returns NULL.
         * @since 0.2.7
         **/
        static Category* exists(const std::string& name);

        /**
         * Returns all the currently defined categories as a vector of
         * Category pointers. Note: this function does not pass ownership
         * of the categories in the vector to the caller, only the ownership
         * of the vector. However vector<Category&>* is not legal C++,
         * so we can't follow the default ownership conventions.
         *
         * <p>Unlike in log4j, the root category <em>is</em> included 
         * in the returned set.
         *
         * @since 0.3.2. Before 0.3.2 this method returned a std::set
         **/
        static std::vector<Category*>* getCurrentCategories();

        /**
         * This method will remove all Appenders from Categories.XXX
         **/
        static void shutdown();

        /**
         * Destructor for Category.
         **/
        virtual ~Category();
        
        /**
         * Return the category name.
         * @returns The category name.
        */       
        virtual const std::string& getName() const throw(); 
        
        /**
         * Set the priority of this Category.
         * @param priority The priority to set. Use Priority::NOTSET to let 
         * the category use its parents priority as effective priority.
         * @exception std::invalid_argument if the caller tries to set
         * Priority::NOTSET on the Root Category.
         **/
        virtual void setPriority(Priority::Value priority) 
        throw(std::invalid_argument);

        /**
         * Returns the assigned Priority, if any, for this Category.
         * @return Priority - the assigned Priority, can be Priority::NOTSET
         **/
        virtual Priority::Value getPriority() const throw();

        /**
         * Starting from this Category, search the category hierarchy for a
         * set priority and return it. Otherwise, return the priority 
         *  of the root category.
         * 
         * <p>The Category class is designed so that this method executes as
         * quickly as possible.
         **/
        virtual Priority::Value getChainedPriority() const throw();

        /** 
         * Returns true if the chained priority of the Category is equal to
         * or higher than given priority.
         * @param priority The priority to compare with.
         * @returns whether logging is enable for this priority.
         **/
        virtual bool isPriorityEnabled(Priority::Value priority) const throw();
        
        /**
         * Adds an Appender to this Category.
         * This method passes ownership from the caller to the Category.
         * @since 0.2.7
         * @param appender The Appender to wich this category has to log.
         * @exception std::invalid_argument if the appender is NULL.
         **/
        virtual void addAppender(Appender* appender) 
        throw(std::invalid_argument);

        /**
         * Adds an Appender for this Category.
         * This method does not pass ownership from the caller to the Category.
         * @since 0.2.7
         * @param appender The Appender this category has to log to.
         **/
        virtual void addAppender(Appender& appender);

        /**
         * Adds an Appender to this Category.
         * This method passes ownership from the caller to the Category.
         * @deprecated use addAppender(Appender*) or removeAllAppenders() 
         * instead.
         * @param appender The Appender this category has to log to or NULL
         * to remove the current Appenders.
         **/
        inline void setAppender(Appender* appender) {
            if (appender) {
                addAppender(appender);
            } else {
                removeAllAppenders();
            }
        };

        /**
         * Adds an Appender for this Category.
         * This method does not pass ownership from the caller to the Category.
         * @deprecated use addAppender(Appender&) instead.
         * @param appender The Appender this category has to log to.
         **/
        inline void setAppender(Appender& appender) {
            addAppender(appender);
        };

        /**
         * Returns the first Appender for this Category, or NULL if no
         * Appender has been set.
         * @deprecated use getAppender(const std::string&)
         * @returns The Appender.
         **/
        virtual Appender* getAppender() const;

        /**
         * Returns the specified Appender for this Category, or NULL if 
         * the Appender is not attached to this Category.
         * @since 0.2.7
         * @returns The Appender.
         **/
        virtual Appender* getAppender(const std::string& name) const;

        /**
         * Returns the set of Appenders currently attached to this Catogory.
         * @since 0.3.1
         * @returns The set of attached Appenders.
         **/
        virtual AppenderSet getAllAppenders() const;

        /**
         * Removes all appenders for this Category.
         **/
        virtual void removeAllAppenders();

        /**
         * Removes specified appender for this Category.
         * @since 0.2.7
         **/
        virtual void removeAppender(Appender* appender);

        /**
         * Returns true if the Category owns the first Appender in its
         * Appender set. In that case the Category destructor will delete
         * the Appender.
         * @deprecated use ownsAppender(Appender*)
         **/
        virtual bool ownsAppender() const throw() {
            return ownsAppender(getAppender());
        };

        /**
         * Returns true if the Category owns the Appender. In that case the
         * Category destructor will delete the Appender.
         * @since 0.2.7
         **/
        virtual bool ownsAppender(Appender* appender) const throw();

        /**
         * Call the appenders in the hierarchy starting at
         *  <code>this</code>.  If no appenders could be found, emit a
         * warning.
         * 
         * <p>This method always calls all the appenders inherited form the
         * hierracy circumventing any evaluation of whether to log or not to
         * log the particular log request.
         * 
         * @param event the LogginEvent to log.
         **/
        virtual void callAppenders(const LoggingEvent& event) throw();
        
        /**
         * Set the additivity flag for this Category instance.
         **/
        virtual void setAdditivity(bool additivity);

        /**
         * Returns the additivity flag for this Category instance.
         **/        
        virtual bool getAdditivity() const throw();

        /**
         * Returns the parent category of this category, or NULL
         * if the category is the root category.
         * @return the parent category.
         **/
        virtual Category* getParent() throw();

        /**
         * Returns the parent category of this category, or NULL
         * if the category is the root category.
         * @return the parent category.
         **/
        virtual const Category* getParent() const throw();

        /** 
         * Log a message with the specified priority.
         * @param priority The priority of this log message.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        virtual void log(Priority::Value priority, const char* stringFormat,
                         ...) throw();

        /** 
         * Log a message with the specified priority.
         * @param priority The priority of this log message.
         * @param message string to write in the log file
         **/  
        virtual void log(Priority::Value priority, 
                         const std::string& message) throw();
        
        /** 
         * Log a message with the specified priority.
         * @since 0.2.7
         * @param priority The priority of this log message.
         * @param stringFormat Format specifier for the string to write
         * in the log file.
         * @param va The arguments for stringFormat.
         **/  
        virtual void logva(Priority::Value priority, 
                           const char* stringFormat,
                           va_list va) throw();
        
        /** 
         * Log a message with debug priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void debug(const char* stringFormat, ...) throw();

        /** 
         * Log a message with debug priority.
         * @param message string to write in the log file
         **/  
        void debug(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority DEBUG.
         * @returns Whether the Category will log.
         **/ 
        inline bool isDebugEnabled() const throw() { 
            return isPriorityEnabled(Priority::DEBUG);
        };
        
        /**
         * Return a CategoryStream with priority DEBUG.
         * @returns The CategoryStream.
         **/
        inline CategoryStream debugStream() {
            return getStream(Priority::DEBUG);
        }

        /** 
         * Log a message with info priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void info(const char* stringFormat, ...) throw();

        /** 
         * Log a message with info priority.
         * @param message string to write in the log file
         **/  
        void info(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority INFO.
         * @returns Whether the Category will log.
         **/ 
        inline bool isInfoEnabled() const throw() { 
            return isPriorityEnabled(Priority::INFO);
        };

        /**
         * Return a CategoryStream with priority INFO.
         * @returns The CategoryStream.
         **/
        inline CategoryStream infoStream() {
            return getStream(Priority::INFO);
        }
        
        /** 
         * Log a message with notice priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void notice(const char* stringFormat, ...) throw();

        /** 
         * Log a message with notice priority.
         * @param message string to write in the log file
         **/  
        void notice(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority NOTICE.
         * @returns Whether the Category will log.
         **/ 
        inline bool isNoticeEnabled() const throw() { 
            return isPriorityEnabled(Priority::NOTICE);
        };

        /**
         * Return a CategoryStream with priority NOTICE.
         * @returns The CategoryStream.
         **/
        inline CategoryStream noticeStream() {
            return getStream(Priority::NOTICE);
        }
        
        /** 
         * Log a message with warn priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void warn(const char* stringFormat, ...) throw();

        /** 
         * Log a message with warn priority.
         * @param message string to write in the log file
         **/  
        void warn(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority WARN.
         * @returns Whether the Category will log.
         **/ 
        inline bool isWarnEnabled() const throw() { 
            return isPriorityEnabled(Priority::WARN);
        };

        /**
         * Return a CategoryStream with priority WARN.
         * @returns The CategoryStream.
         **/
        inline CategoryStream warnStream() {
            return getStream(Priority::WARN);
        };
        
        /** 
         * Log a message with error priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void error(const char* stringFormat, ...) throw();

        /** 
         * Log a message with error priority.
         * @param message string to write in the log file
         **/  
        void error(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority ERROR.
         * @returns Whether the Category will log.
         **/ 
        inline bool isErrorEnabled() const throw() { 
            return isPriorityEnabled(Priority::ERROR);
        };
        
        /**
         * Return a CategoryStream with priority ERROR.
         * @returns The CategoryStream.
         **/
        inline CategoryStream errorStream() {
            return getStream(Priority::ERROR);
        };

        /** 
         * Log a message with crit priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void crit(const char* stringFormat, ...) throw();

        /** 
         * Log a message with crit priority.
         * @param message string to write in the log file
         **/  
        void crit(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority CRIT.
         * @returns Whether the Category will log.
         **/ 
        inline bool isCritEnabled() const throw() { 
            return isPriorityEnabled(Priority::CRIT);
        };
        
        /**
         * Return a CategoryStream with priority CRIT.
         * @returns The CategoryStream.
         **/
        inline CategoryStream critStream() {
            return getStream(Priority::CRIT);
        };
        
        /** 
         * Log a message with alert priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void alert(const char* stringFormat, ...) throw();

        /** 
         * Log a message with alert priority.
         * @param message string to write in the log file
         **/  
        void alert(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority ALERT.
         * @returns Whether the Category will log.
         **/ 
        inline bool isAlertEnabled() const throw() { 
            return isPriorityEnabled(Priority::ALERT);
        };
        
        /**
         * Return a CategoryStream with priority ALERT.
         * @returns The CategoryStream.
         **/
        inline CategoryStream alertStream() throw() {
            return getStream(Priority::ALERT);
        };

        /** 
         * Log a message with emerg priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void emerg(const char* stringFormat, ...) throw();

        /** 
         * Log a message with emerg priority.
         * @param message string to write in the log file
         **/  
        void emerg(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority EMERG.
         * @returns Whether the Category will log.
         **/ 
        inline bool isEmergEnabled() const throw() { 
            return isPriorityEnabled(Priority::EMERG);
        };
        
        /**
         * Return a CategoryStream with priority EMERG.
         * @returns The CategoryStream.
         **/
        inline CategoryStream emergStream() {
            return getStream(Priority::EMERG);
        };

        /** 
         * Log a message with fatal priority. 
         * NB. priority 'fatal' is equivalent to 'emerg'.
         * @since 0.2.7
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void fatal(const char* stringFormat, ...) throw();

        /** 
         * Log a message with fatal priority.
         * NB. priority 'fatal' is equivalent to 'emerg'.
         * @since 0.2.7
         * @param message string to write in the log file
         **/  
        void fatal(const std::string& message) throw();

        /**
         * Return true if the Category will log messages with priority FATAL.
         * NB. priority 'fatal' is equivalent to 'emerg'.
         * @since 0.2.7
         * @returns Whether the Category will log.
         **/ 
        inline bool isFatalEnabled() const throw() { 
            return isPriorityEnabled(Priority::FATAL);
        };
        
        /**
         * Return a CategoryStream with priority FATAL.
         * NB. priority 'fatal' is equivalent to 'emerg'.
         * @since 0.2.7
         * @returns The CategoryStream.
         **/
        inline CategoryStream fatalStream() {
            return getStream(Priority::FATAL);
        };

        /**
         * Return a CategoryStream with given Priority.
         * @param priority The Priority of the CategoryStream.
         * @returns The requested CategoryStream.
         **/
        virtual CategoryStream getStream(Priority::Value priority);

        /**
         * Return a CategoryStream with given Priority.
         * @param priority The Priority of the CategoryStream.
         * @returns The requested CategoryStream.
         **/
        virtual CategoryStream operator<<(Priority::Value priority);

        protected:

        /**
         * Constructor 
         * @param name the fully qualified name of this Category
         * @param parent the parent of this parent, or NULL for the root 
         * Category
         * @param priority the priority for this Category. Defaults to
         * Priority::NOTSET
         **/
        Category(const std::string& name, Category* parent, 
                                Priority::Value priority = Priority::NOTSET);
        
        virtual void _logUnconditionally(Priority::Value priority, 
                                         const char* format, 
                                         va_list arguments) throw();
        
        /** 
         * Unconditionally log a message with the specified priority.
         * @param priority The priority of this log message.
         * @param message string to write in the log file
         **/  
        virtual void _logUnconditionally2(Priority::Value priority, 
                                          const std::string& message) throw();

        private:

        /* prevent copying and assignment */
        Category(const Category& other);
        Category& operator=(const Category& other);

        /** The name of this category. */
        const std::string _name;

        /**
         * The parent of this category. All categories have al least one
         * ancestor which is the root category. 
         **/
        Category* _parent;

        /**
         *  The assigned priority of this category. 
         **/
        volatile Priority::Value _priority;

        typedef std::map<Appender *, bool> OwnsAppenderMap;

        /**
         * Returns the iterator to the Appender if the Category owns the
         * Appender. In that case the Category destructor will delete the
         * Appender.
         **/

        virtual bool ownsAppender(Appender* appender, 
                                  OwnsAppenderMap::iterator& i2) throw();

        AppenderSet _appender;
        mutable threading::Mutex _appenderSetMutex;

        /**
         * Whether the category holds the ownership of the appender. If so,
         * it deletes the appender in its destructor.
         **/
        
        OwnsAppenderMap _ownsAppender;

        /** 
         * Additivity is set to true by default, i.e. a child inherits its
         * ancestor's appenders by default. 
         */
        volatile bool _isAdditive;

    };

}
#endif // _LOG4CPP_CATEGORY_HH
