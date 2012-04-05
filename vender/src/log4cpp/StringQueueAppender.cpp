/*
 * StringQueueAppender.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/StringQueueAppender.hh>

namespace log4cpp {

    StringQueueAppender::StringQueueAppender(const std::string& name) : 
        LayoutAppender(name) {
    }
    
    StringQueueAppender::~StringQueueAppender() {
        close();
    }

    void StringQueueAppender::close() {
        // empty
    }

    void StringQueueAppender::_append(const LoggingEvent& event) {
        _queue.push(_getLayout().format(event));
    }

    bool StringQueueAppender::reopen() {
        return true;
    }      

    std::queue<std::string>& StringQueueAppender::getQueue() {
        return _queue;
    }

    const std::queue<std::string>& StringQueueAppender::getQueue() const {
        return _queue;
    }

    size_t StringQueueAppender::queueSize() const {
        return getQueue().size();
    }

    std::string StringQueueAppender::popMessage() {        
        std::string message;

        if (!_queue.empty()) {
            message = _queue.front();
            _queue.pop();
        }

        return message;
    }
}
