/*
 * Filter.cpp
 *
 * Copyright 2001, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2001, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/Filter.hh>

namespace log4cpp {

    Filter::Filter() :
        _chainedFilter(NULL) {
    }

    Filter::~Filter() {
        if (_chainedFilter)
            delete _chainedFilter;
    }

    void Filter::setChainedFilter(Filter* filter) {
        if (filter != _chainedFilter) {
            if (_chainedFilter)
                delete _chainedFilter;
            
            _chainedFilter = filter;
        }
    }

    Filter* Filter::getChainedFilter() {
        return _chainedFilter;
    }

    Filter* Filter::getEndOfChain() {
        Filter* end = this;
        while(end->getChainedFilter()) {
            end = end->getChainedFilter();
        }
	return end;
    }

    void Filter::appendChainedFilter(Filter* filter) {
        Filter* end = getEndOfChain();
        end->setChainedFilter(filter);
    }

    Filter::Decision Filter::decide(const LoggingEvent& event) {
        Filter::Decision decision = _decide(event);

        if ((Filter::NEUTRAL == decision) && getChainedFilter()) {
            decision = getChainedFilter()->decide(event);
        }

        return decision;
    }
    
}
