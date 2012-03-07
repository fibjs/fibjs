/*
 * StringUtil.cpp
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */
#include "StringUtil.hh"
#include <iterator>
#include <stdio.h>

#if defined(_MSC_VER)
    #define VSNPRINTF _vsnprintf
#else
#ifdef LOG4CPP_HAVE_SNPRINTF
    #define VSNPRINTF vsnprintf
#else
/* use alternative snprintf() from http://www.ijs.si/software/snprintf/ */

#define HAVE_SNPRINTF
#define PREFER_PORTABLE_SNPRINTF

#include <stdlib.h>
#include <stdarg.h>

extern "C" {
#include "snprintf.c"
}

#define VSNPRINTF portable_vsnprintf

#endif // LOG4CPP_HAVE_SNPRINTF
#endif // _MSC_VER

namespace log4cpp {

    std::string StringUtil::vform(const char* format, va_list args) {
	size_t size = 1024;
	char* buffer = new char[size];

	while (1) {
	    int n = VSNPRINTF(buffer, size, format, args);

	    // If that worked, return a string.
	    if ((n > -1) && (static_cast<size_t>(n) < size)) {
		std::string s(buffer);
		delete [] buffer;
		return s;
	    }

	    // Else try again with more space.
            size = (n > -1) ?
                n + 1 :   // ISO/IEC 9899:1999
                size * 2; // twice the old size

	    delete [] buffer;
	    buffer = new char[size];
	}
    }

    std::string StringUtil::trim(const std::string& s) {
        static const char* whiteSpace = " \t\r\n";

        // test for null string
        if(s.empty())
            return s;

        // find first non-space character
        std::string::size_type b = s.find_first_not_of(whiteSpace);
        if(b == std::string::npos) // No non-spaces
            return "";

        // find last non-space character
        std::string::size_type e = s.find_last_not_of(whiteSpace);

        // return the remaining characters
        return std::string(s, b, e - b + 1);
    }

    unsigned int StringUtil::split(std::vector<std::string>& v,
				   const std::string& s,
				   char delimiter, unsigned int maxSegments) {
        v.clear();
        std::back_insert_iterator<std::vector<std::string> > it(v);
        return split(it, s, delimiter, maxSegments);
    }

}
