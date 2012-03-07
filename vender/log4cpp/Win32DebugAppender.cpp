/*
 * Win32DebugAppender.cpp
 *
 * Copyright 2002, the Log4cpp project.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifdef WIN32    // only available on Win32

#include "PortabilityImpl.hh"
#ifdef LOG4CPP_HAVE_IO_H
#    include <io.h>
#endif

#include <windows.h>
#include <winbase.h>

#include <stdio.h>
#include "log4cpp/Category.hh"
#include "log4cpp/Win32DebugAppender.hh"
#include <log4cpp/FactoryParams.hh>
#include <memory>

namespace log4cpp {

    Win32DebugAppender::Win32DebugAppender(const std::string& name) : 
            LayoutAppender(name) {
    }
    
    Win32DebugAppender::~Win32DebugAppender() {
        close();
    }

    void Win32DebugAppender::close() {
    }

    void Win32DebugAppender::_append(const LoggingEvent& event) {
        std::string message(_getLayout().format(event));
                ::OutputDebugString(message.c_str());
    }

    std::auto_ptr<Appender> create_win32_debug_appender(const FactoryParams& params)
    {
       std::string name;
       params.get_for("win32 debug appender").required("name", name);
       
       return std::auto_ptr<Appender>(new Win32DebugAppender(name));
    }
}

#endif  // WIN32
