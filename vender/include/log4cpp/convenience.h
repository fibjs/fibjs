#ifndef LOG4CPP_CONVENIENCE_H
#define LOG4CPP_CONVENIENCE_H

#define LOG4CPP_LOGGER(name) \
  static log4cpp::Category& logger = log4cpp::Category::getInstance( name );

#define LOG4CPP_LOGGER_N(var_name, name) \
  static log4cpp::Category& var_name = log4cpp::Category::getInstance( name );

// simple logging
#define LOG4CPP_EMERG(logger, msg) \
   if (logger.isEmergEnabled()) logger.emerg( msg );

#define LOG4CPP_FATAL(logger, msg) \
   if (logger.isFatalEnabled()) logger.fatal( msg );

#define LOG4CPP_ALERT(logger, msg) \
   if (logger.isAlertEnabled()) logger.alert( msg );

#define LOG4CPP_CRIT(logger, msg) \
   if (logger.isCritEnabled()) logger.crit( msg );

#define LOG4CPP_ERROR(logger, msg) \
   if (logger.isErrorEnabled()) logger.error( msg );

#define LOG4CPP_WARN(logger, msg) \
   if (logger.isWarnEnabled()) logger.warn( msg );

#define LOG4CPP_NOTICE(logger, msg) \
   if (logger.isNoticeEnabled()) logger.notice( msg );

#define LOG4CPP_INFO(logger, msg) \
   if (logger.isInfoEnabled()) logger.info( msg );

#define LOG4CPP_DEBUG(logger, msg) \
   if (logger.isDebugEnabled()) logger.debug( msg );


// stream logging
#define LOG4CPP_EMERG_S(logger) \
   if (logger.isEmergEnabled()) logger.emergStream()

#define LOG4CPP_FATAL_S(logger) \
   if (logger.isFatalEnabled()) logger.fatalStream()

#define LOG4CPP_ALERT_S(logger) \
   if (logger.isAlertEnabled()) logger.alertStream()

#define LOG4CPP_CRIT_S(logger) \
   if (logger.isCritEnabled()) logger.critStream()

#define LOG4CPP_ERROR_S(logger) \
   if (logger.isErrorEnabled()) logger.errorStream()

#define LOG4CPP_WARN_S(logger) \
   if (logger.isWarnEnabled()) logger.warnStream()

#define LOG4CPP_NOTICE_S(logger) \
   if (logger.isNoticeEnabled()) logger.noticeStream()

#define LOG4CPP_INFO_S(logger) \
   if (logger.isInfoEnabled()) logger.infoStream()

#define LOG4CPP_DEBUG_S(logger) \
   if (logger.isDebugEnabled()) logger.debugStream()


// stream logging with default logger "logger"
#define LOG4CPP_EMERG_SD() \
   if (logger.isEmergEnabled()) logger.emergStream()

#define LOG4CPP_FATAL_SD() \
   if (logger.isFatalEnabled()) logger.fatalStream()

#define LOG4CPP_ALERT_SD() \
   if (logger.isAlertEnabled()) logger.alertStream()

#define LOG4CPP_CRIT_SD() \
   if (logger.isCritEnabled()) logger.critStream()

#define LOG4CPP_ERROR_SD() \
   if (logger.isErrorEnabled()) logger.errorStream()

#define LOG4CPP_WARN_SD() \
   if (logger.isWarnEnabled()) logger.warnStream()

#define LOG4CPP_NOTICE_SD() \
   if (logger.isNoticeEnabled()) logger.noticeStream()

#define LOG4CPP_INFO_SD() \
   if (logger.isInfoEnabled()) logger.infoStream()

#define LOG4CPP_DEBUG_SD() \
   if (logger.isDebugEnabled()) logger.debugStream()

#endif

