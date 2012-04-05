/*
 * Properties.cpp
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "Properties.hh"
#include <cstdlib>
#include "StringUtil.hh"

namespace log4cpp {
    
    Properties::Properties() {
    }

    Properties::~Properties() {
    }

    void Properties::load(std::istream& in) {
        clear();

        std::string fullLine, command;
        std::string leftSide, rightSide;
        char line[256];
        std::string::size_type length;

        while (in.getline(line, 256)) {
            fullLine = line;

            /* if the line contains a # then it is a comment
               if we find it anywhere other than the beginning, then we assume 
               there is a command on that line, and it we don't find it at all
               we assume there is a command on the line (we test for valid 
               command later) if neither is true, we continue with the next line
            */
            length = fullLine.find('#');
            if (length == std::string::npos) {
                command = fullLine;
            } else if (length > 0) {
                command = fullLine.substr(0, length);
            } else {
                continue;
            }

            // check the command and handle it
            length = command.find('=');
            if (length != std::string::npos) {
                leftSide = StringUtil::trim(command.substr(0, length));
                rightSide = StringUtil::trim(command.substr(length + 1, command.size() - length));
                _substituteVariables(rightSide);
            } else {
                continue;
            }

            /* handle the command by determining what object the left side
               refers to and setting the value given on the right
               ASSUMPTIONS:  
               1. first object given  on left side is "log4j" or "log4cpp"
               2. all class names on right side are ignored because we
               probably cannot resolve them anyway.
            */

            // strip off the "log4j" or "log4cpp"
            length = leftSide.find('.');
            if (leftSide.substr(0, length) == "log4j" ||
                leftSide.substr(0, length) == "log4cpp")
                leftSide = leftSide.substr(length + 1);

            // add to the map of properties
            insert(value_type(leftSide, rightSide));
        }
    }

    void Properties::save(std::ostream& out) {
        for(const_iterator i = begin(); i != end(); ++i) {
            out << (*i).first << "=" << (*i).second << std::endl;
        }
    }
    
    int Properties::getInt(const std::string& property, int defaultValue) {
        const_iterator key = find(property);
        return (key == end()) ? defaultValue : std::atoi((*key).second.c_str());
    }

    bool Properties::getBool(const std::string& property, bool defaultValue) {
        const_iterator key = find(property);
        return (key == end()) ? defaultValue : ((*key).second == "true");
    }

    std::string Properties::getString(const std::string& property, 
                                      const char* defaultValue) {
        const_iterator key = find(property);
        return (key == end()) ? std::string(defaultValue) : (*key).second;
    }

    void Properties::_substituteVariables(std::string& value) {
        std::string result;

        std::string::size_type left = 0;
        std::string::size_type right = value.find("${", left);
        if (right == std::string::npos) {
            // bail out early for 99% of cases
            return;
        }

        while(true) {
            result += value.substr(left, right - left);
            if (right == std::string::npos) {
                break;
            }

            left = right + 2;
            right = value.find('}', left);
            if (right == std::string::npos) {
                // no close tag, use string literally
                result += value.substr(left - 2);
                break;
            } else {
                const std::string key = value.substr(left, right - left);
                if (key == "${") {
                    result += "${";
                } else {
                    char* value = std::getenv(key.c_str());
                    if (value) {
                        result += value;
                    } else {
                        const_iterator it = find(key);
                        if (it == end()) {
                            // not found assume empty;
                        } else {
                            result += (*it).second;
                        }
                    }
                }
                left = right + 1;
            }

            right = value.find("${", left);
        }

        value = result;
    }
}
