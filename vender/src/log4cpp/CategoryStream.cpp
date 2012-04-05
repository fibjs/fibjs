/*
 * CategoryStream.cpp
 *
 * Copyright 2001, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2001, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"

#ifdef LOG4CPP_HAVE_UNISTD_H
#    include <unistd.h>
#endif

#include <log4cpp/CategoryStream.hh>
#include <log4cpp/Category.hh>

namespace log4cpp {

    CategoryStream::CategoryStream(Category& category, Priority::Value priority) :
        _category(category),
        _priority(priority),
        _buffer(NULL) {
    }

    CategoryStream::~CategoryStream() { 
        flush();
    }

    void CategoryStream::flush() {
        if (_buffer) {
            getCategory().log(getPriority(), _buffer->str());
            delete _buffer;
            _buffer = NULL;
        }
    }
     std::streamsize CategoryStream::width(std::streamsize wide ) {
        if (getPriority() != Priority::NOTSET) {
            if (!_buffer) {
                if (!(_buffer = new std::ostringstream)) {
                    // XXX help help help
                }
            }
        }
        return _buffer->width(wide); 
    }
    CategoryStream& CategoryStream::operator<< (cspf pf) {
		return (*pf)(*this);
    }
    CategoryStream& eol (CategoryStream& os) {
        if  (os._buffer) {
		os.flush();
        }
        return os;
    }
    CategoryStream& left (CategoryStream& os) {
        if  (os._buffer) {
            os._buffer->setf(std::ios::left);
        }
        return os;
    }
}
