#include <log4cpp/threading/Threading.hh>

#ifndef LOG4CPP_HAVE_THREADING

namespace log4cpp {
    namespace threading {

        std::string getThreadId() {
            // more useful would be to return the PID.
            return std::string("thread1");
        }

    }
}

#endif // LOG4CPP_HAVE_THREADING
