/*
 * Win32DebugAppender.hh
 *
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_WIN32DEBUGAPPENDER_HH
#define _LOG4CPP_WIN32DEBUGAPPENDER_HH

#ifdef WIN32	// only use this on Win32

#include <string>
#include "log4cpp/Export.hh"
#include "log4cpp/LayoutAppender.hh"

namespace log4cpp {

	/**
	 * Win32DebugAppender simply sends the log message to the default system
	 * debugger on Win32 systems.  This is useful for users of MSVC and Borland
	 * because the log messages will show up in the debugger window.<BR>
         * <B>NB:</B> This class is only available on Win32 platforms.
	 */
    class LOG4CPP_EXPORT Win32DebugAppender : public LayoutAppender {
        public:
		/**
		 * Constructor.
		 * @param name Name used by the base classes only.
		 */
        Win32DebugAppender(const std::string& name);
		/**
		 * Destructor.
		 */
        virtual ~Win32DebugAppender();
        
		/**
		 * Close method.  This is called by the framework, but there is nothing
		 * to do for the OutputDebugString API, so it simply returns.
		 */
        virtual void close();

        protected:
		/**
		 * Method that does the actual work.  In this case, it simply sets up the layout
		 * and calls the OutputDebugString API.
		 * @param event Event for which we are logging.
		 */
        virtual void _append(const LoggingEvent& event);
    };
}

#else // WIN32
#error NTEventLoggAppender is not available on on Win32 platforms
#endif	// WIN32

#endif // _LOG4CPP_WIN32DEBUGAPPENDER_HH
