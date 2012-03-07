/*
 * FixedContextCategory.cpp
 *
 * Copyright 2001, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2001, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/FixedContextCategory.hh>

namespace log4cpp {

    FixedContextCategory::FixedContextCategory(const std::string& name,
                                               const std::string& context) : 
        Category(name, Category::getInstance(name).getParent()),
        _delegate(Category::getInstance(name)),
        _context(context) {
    }

    FixedContextCategory::~FixedContextCategory() {
    }

    void FixedContextCategory::setContext(const std::string& context) {
        _context = context;
    }

    std::string FixedContextCategory::getContext() const {
        return _context;
    }

    Priority::Value FixedContextCategory::getPriority() const throw() {
        return Category::getPriority();
    }
   
    Priority::Value FixedContextCategory::getChainedPriority() const throw() {
        Priority::Value result = getPriority();

        if (result == Priority::NOTSET) {
            result = _delegate.getChainedPriority();
        }

        return result;
    }
    
    void FixedContextCategory::addAppender(Appender* appender) throw() {
        // XXX do nothing for now
    }
    
    void FixedContextCategory::addAppender(Appender& appender) {
        // XXX do nothing for now
    }
    
    Appender* FixedContextCategory::getAppender() const {
        return _delegate.getAppender();
    }
    
    Appender* FixedContextCategory::getAppender(const std::string& name)
    const {
        return _delegate.getAppender(name);
    }

    AppenderSet FixedContextCategory::getAllAppenders() const {
        return _delegate.getAllAppenders();
    }

    void FixedContextCategory::removeAllAppenders() {
        // XXX do nothing for now
    }

    bool FixedContextCategory::ownsAppender() const throw() {
        return false;
    }
    
    bool FixedContextCategory::ownsAppender(Appender* appender) const throw() {
        return false;
    }
    
    void FixedContextCategory::callAppenders(const LoggingEvent& event)
            throw() {
        _delegate.callAppenders(event);
    }

    void FixedContextCategory::setAdditivity(bool additivity) {
        // XXX do nothing for now
    }

    bool FixedContextCategory::getAdditivity() const throw() {
        return _delegate.getAdditivity();
    }

    void FixedContextCategory::_logUnconditionally2(Priority::Value priority,
            const std::string& message) throw() {
        LoggingEvent event(getName(), message, _context, priority);
        callAppenders(event);
    }
    
} 

