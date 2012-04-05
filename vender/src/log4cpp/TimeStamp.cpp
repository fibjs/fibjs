/*
 * TimeStamp.cpp
 *
 * Copyright 2001, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2001, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4cpp/TimeStamp.hh>

#include <cstring>

#ifdef LOG4CPP_HAVE_GETTIMEOFDAY
#include <sys/time.h>
#else
#ifdef LOG4CPP_HAVE_FTIME
#include <sys/timeb.h>
#else
#include <time.h>
#endif
#endif

namespace log4cpp {

    LOG4CPP_EXPORT TimeStamp TimeStamp::_startStamp;

    TimeStamp::TimeStamp() {
#ifdef LOG4CPP_HAVE_GETTIMEOFDAY
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        _seconds = tv.tv_sec;
        _microSeconds = tv.tv_usec;
#else
#ifdef LOG4CPP_HAVE_FTIME
        struct timeb tb;
        ::ftime(&tb);
        _seconds = tb.time;
        _microSeconds = 1000 * tb.millitm;
#else
        _seconds = ::time(NULL);
        _microSeconds = 0;
#endif
#endif
    }

    TimeStamp::TimeStamp(unsigned int seconds, unsigned int microSeconds) :
        _seconds(seconds),
        _microSeconds(microSeconds) {
    }
}


