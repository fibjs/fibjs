#include <log4cpp/threading/Threading.hh>

#if defined(LOG4CPP_HAVE_THREADING) && defined(LOG4CPP_USE_PTHREADS)

namespace log4cpp {
    namespace threading {

        std::string getThreadId() {
            char buffer[16];
            ::sprintf(buffer, "%ld", pthread_self());
            return std::string(buffer);
        }

    }
}

#endif // LOG4CPP_HAVE_THREADING && LOG4CPP_USE_PTHREADS
