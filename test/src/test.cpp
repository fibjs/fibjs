#include "test.h"

void pcl_main();
void v8_main();

int main(int argc, char **argv)
{
    try
    {
        log4cpp::PropertyConfigurator::configure("log.ini");
    }
    catch (log4cpp::ConfigureFailure e)
    {
        log4cpp::Category& root = log4cpp::Category::getRoot();

        log4cpp::Appender* appender = new log4cpp::OstreamAppender("console", &std::cout);
        appender->setLayout(new log4cpp::BasicLayout());
        root.addAppender(appender);
        root.setPriority(log4cpp::Priority::DEBUG);

        root.warn(e.what());
    }

    pcl_main();
	v8_main();
    return 0;
}
