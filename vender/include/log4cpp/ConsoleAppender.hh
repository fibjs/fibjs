#ifndef _LOG4CPP_CONSOLEAPPENDER_HH
#define _LOG4CPP_CONSOLEAPPENDER_HH

#include <log4cpp/OstreamAppender.hh>

namespace log4cpp
{

typedef Appender *(*consoleGetter)(const std::string &);

Appender *get_console(const std::string &appenderName);
void set_ConsoleGetter(consoleGetter cg);

};

#endif // _LOG4CPP_CONSOLEAPPENDER_HH
