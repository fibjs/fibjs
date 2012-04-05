/*
 * BasicConfigurator.cpp
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */
#include "PortabilityImpl.hh"

#ifdef LOG4CPP_HAVE_IO_H
#    include <io.h>
#endif
#ifdef LOG4CPP_HAVE_UNISTD_H
#    include <unistd.h>
#endif

#include <cstdio>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>

namespace log4cpp {

    void BasicConfigurator::configure() {
        Category& root = Category::getRoot();
        root.setPriority(Priority::INFO);
        root.removeAllAppenders();
        root.addAppender(new FileAppender("_", ::dup(fileno(stdout))));
    }

}



