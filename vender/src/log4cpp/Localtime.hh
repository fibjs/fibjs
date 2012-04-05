#ifndef _LOG4CPP_LOCALTIME_HH
#define _LOG4CPP_LOCALTIME_HH

#include <time.h>

namespace log4cpp
{
   void localtime(const ::time_t* time, ::tm* t);
}

#endif
