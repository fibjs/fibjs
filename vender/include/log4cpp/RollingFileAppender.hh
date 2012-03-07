/*
 * RollingFileAppender.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_ROLLINGFILEAPPENDER_HH
#define _LOG4CPP_ROLLINGFILEAPPENDER_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/FileAppender.hh>
#include <string>
#include <stdarg.h>

namespace log4cpp {

    /**
       RollingFileAppender is a FileAppender that rolls over the logfile once
       it has reached a certain size limit.
       @since 0.3.1
    **/
    class LOG4CPP_EXPORT RollingFileAppender : public FileAppender {
        public:
        RollingFileAppender(const std::string& name, 
                            const std::string& fileName,
                            size_t maxFileSize = 10*1024*1024, 
                            unsigned int maxBackupIndex = 1,
                            bool append = true,
                            mode_t mode = 00644);

        virtual void setMaxBackupIndex(unsigned int maxBackups);
        virtual unsigned int getMaxBackupIndex() const;
        virtual void setMaximumFileSize(size_t maxFileSize);
        virtual size_t getMaxFileSize() const;

        virtual void rollOver();

        protected:
        virtual void _append(const LoggingEvent& event);

        unsigned int _maxBackupIndex;
        size_t _maxFileSize;
    };
}

#endif // _LOG4CPP_ROLLINGFILEAPPENDER_HH
