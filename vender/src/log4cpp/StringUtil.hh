/*
 * StringUtil.hh
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_STRINGUTIL_HH
#define _LOG4CPP_STRINGUTIL_HH

#include "PortabilityImpl.hh"
#include <string>
#include <vector>
#include <climits>
#include <stdarg.h>

namespace log4cpp {

    class StringUtil {
        public:

        /**
           Returns a string contructed from the a format specifier
           and a va_list of arguments, analogously to vprintf(3).
           @param format the format specifier.
           @param args the va_list of arguments.
        **/
        static std::string vform(const char* format, va_list args);

        /**
           Returns a string identical to the given string but without leading
           or trailing HTABs or spaces.
        **/
        static std::string trim(const std::string& s);

        /**
           splits a string into a vector of string segments based on the
           given delimiter.
           @param v The vector in which the segments will be stored. The vector
           will be emptied before storing the segments
           @param s The string to split into segments.
           @param delimiter The delimiter character
           @param maxSegments the maximum number of segments. Upon return
           v.size() <= maxSegments.  The string is scanned from left to right
           so v[maxSegments - 1] may contain a string containing the delimiter
           character.
	   @return The actual number of segments (limited by maxSegments).
        **/
        static unsigned int split(std::vector<std::string>& v, 
				  const std::string& s, char delimiter,
				  unsigned int maxSegments = INT_MAX);
        /**
           splits a string into string segments based on the given delimiter
           and assigns the segments through an output_iterator.
           @param output The output_iterator through which to assign the string
           segments. Typically this will be a back_insertion_iterator.
           @param s The string to split into segments.
           @param delimiter The delimiter character
           @param maxSegments The maximum number of segments.
           @return The actual number of segments (limited by maxSegments).
        **/
        template<typename T> static unsigned int split(T& output,
                const std::string& s, char delimiter,
                unsigned int maxSegments = INT_MAX) {
            std::string::size_type left = 0;
            unsigned int i;
            for(i = 1; i < maxSegments; i++) {
                std::string::size_type right = s.find(delimiter, left);
                if (right == std::string::npos) {
                    break;
                }
                *output++ = s.substr(left, right - left);
                left = right + 1;
            }
            
            *output++ = s.substr(left);
            return i;
        }
    };
}

#endif // _LOG4CPP_STRINGUTIL_HH

