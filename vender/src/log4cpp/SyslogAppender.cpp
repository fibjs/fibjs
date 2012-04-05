/*
 * SyslogAppender.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#if LOG4CPP_HAVE_SYSLOG

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log4cpp/SyslogAppender.hh>
#include <log4cpp/FactoryParams.hh>
#include <memory>

namespace log4cpp {

    int SyslogAppender::toSyslogPriority(Priority::Value priority) {
        static int priorities[8] = { LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
                                     LOG_WARNING, LOG_NOTICE, LOG_INFO, 
                                     LOG_DEBUG };
        int result;

        priority++;
        priority /= 100;

        if (priority < 0) {
            result = LOG_EMERG;
        } else if (priority > 7) {
            result = LOG_DEBUG;
        } else {
            result = priorities[priority];
        }

        return result;
    }
        

    SyslogAppender::SyslogAppender(const std::string& name, 
                                   const std::string& syslogName, 
                                   int facility) : 
        LayoutAppender(name),
        _syslogName(syslogName),
        _facility(facility) 
    {
        open();
    }
    
    SyslogAppender::~SyslogAppender() {
        close();
    }

    void SyslogAppender::open() {
        openlog(_syslogName.c_str(), 0, _facility);
    }

    void SyslogAppender::close() {
        ::closelog();
    }

    void SyslogAppender::_append(const LoggingEvent& event) {
        std::string message(_getLayout().format(event));
        int priority = toSyslogPriority(event.priority);
        ::syslog(priority | _facility, "%s", message.c_str());
    }

    bool SyslogAppender::reopen() {
        close();
        open();
        return true;
    }
    
    std::auto_ptr<Appender> create_syslog_appender(const FactoryParams& params)
    {
       std::string name, syslog_name;
       int facility = 0;
       params.get_for("syslog appender").required("name", name)("syslog_name", syslog_name)
                                        .optional("facility", facility);
       return std::auto_ptr<Appender>(new SyslogAppender(name, syslog_name, facility));
    }
}

#endif // LOG4CPP_HAVE_SYSLOG
