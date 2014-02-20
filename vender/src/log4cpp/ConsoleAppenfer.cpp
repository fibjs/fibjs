
#include <log4cpp/ConsoleAppender.hh>

namespace log4cpp
{

static consoleGetter s_cg;

void set_ConsoleGetter(consoleGetter cg)
{
    s_cg = cg;
}

Appender *get_console(const std::string &appenderName)
{
    if (s_cg)
        return s_cg(appenderName);
    return new OstreamAppender(appenderName, &std::cout);
}

};
