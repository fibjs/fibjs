#include <log4cpp/threading/Threading.hh>

#if defined(LOG4CPP_HAVE_THREADING) && defined(LOG4CPP_USE_OMNITHREADS)

namespace log4cpp {
    namespace threading {

        std::string getThreadId() {
            char buffer[16];
            sprintf(buffer, "%d", ::omni_thread::self()->id());
            return std::string(buffer);
        };
    }
}

#endif // LOG4CPP_HAVE_THREADING && LOG4CPP_USE_ONMITHREADS
