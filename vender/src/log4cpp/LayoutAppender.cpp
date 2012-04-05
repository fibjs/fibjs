/*
 * LayoutAppender.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/LayoutAppender.hh>

namespace log4cpp {

    LayoutAppender::LayoutAppender(const std::string& name) : 
            AppenderSkeleton(name),
            _layout(new DefaultLayoutType()) {
    }
    
    LayoutAppender::~LayoutAppender() {
        delete _layout;
    }

    bool LayoutAppender::requiresLayout() const {
        return true;
    }

    void LayoutAppender::setLayout(Layout* layout) {
       if (layout != _layout) {
	  Layout *oldLayout = _layout;
	  _layout = (layout == NULL) ? new DefaultLayoutType() : layout;
	  delete oldLayout;
       }
    }

    Layout& LayoutAppender::_getLayout() {
        return *_layout;
    }
}
