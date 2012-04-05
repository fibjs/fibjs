/*
 * Priority.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/Priority.hh>
#include <cstdlib>

namespace log4cpp {

    namespace {
        const std::string names[10] = {
            "FATAL",
			"ALERT",
			"CRIT",
			"ERROR",
			"WARN",
            "NOTICE",
			"INFO",
			"DEBUG",
			"NOTSET",
			"UNKNOWN" 
        };
    }    

    const int log4cpp::Priority::MESSAGE_SIZE = 8;
    

    const std::string& Priority::getPriorityName(int priority) throw() {
         
        priority++;
        priority /= 100;
        return names[((priority < 0) || (priority > 8)) ? 8 : priority];
    }

    Priority::Value Priority::getPriorityValue(const std::string& priorityName) 
    throw(std::invalid_argument) {
	Priority::Value value = -1;

	for (unsigned int i = 0; i < 10; i++) {
	    if (priorityName == names[i]) {
		value = i * 100;
		break;
	    }
	}

	if (value == -1) {
	    if (priorityName == "EMERG") {
		    value = 0;
	    } else {
		    char* endPointer;
		    value = std::strtoul(priorityName.c_str(), &endPointer, 10);
		    if (*endPointer != 0) {
		        throw std::invalid_argument(
					std::string("unknown priority name: '") + priorityName + "'"
				);
		    }
	    }
	}
	
	return value;
    }
}
