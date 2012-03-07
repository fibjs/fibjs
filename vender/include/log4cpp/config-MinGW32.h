#ifndef _INCLUDE_LOG4CPP_CONFIG_MINGW32_H
#define _INCLUDE_LOG4CPP_CONFIG_MINGW32_H 1

/* manually edited from include/log4cpp/config.h */

/* Define if you have the syslog function.  */
/* #undef LOG4CPP_HAVE_SYSLOG */

/* Define if you have the `ftime' function. */
#ifndef LOG4CPP_HAVE_FTIME
#define LOG4CPP_HAVE_FTIME  1
#endif

/* Define if you have the `gettimeofday' function. */
/* #undef LOG4CPP_HAVE_GETTIMEOFDAY */

/* define if the compiler has int64_t */
#ifndef LOG4CPP_HAVE_INT64_T
#define LOG4CPP_HAVE_INT64_T
#define int64_t __int64

/* define if the compiler has in_addr_t */
#ifndef LOG4CPP_HAVE_IN_ADDR_T
#define LOG4CPP_HAVE_IN_ADDR_T

#ifndef u_long
typedef unsigned long u_long;
#endif

/* u_long is the type of in_addr.s_addr */
typedef u_long in_addr_t;

/* u_short is the type of sockaddr_in.sin_port */
// typedef u_short              in_port_t;

#endif

#endif

/* Define if you have the <io.h> header file.  */
#ifndef LOG4CPP_HAVE_IO_H
#define LOG4CPP_HAVE_IO_H 1
#endif

/* Define if you have the <unistd.h> header file.  */
/* #undef LOG4CPP_HAVE_UNISTD_H */

/* Define if you have the idsa library (-lidsa).  */
/* #undef LOG4CPP_HAVE_LIBIDSA */

/* Define if you have the `strcasecmp' function. */
/* #undef LOG4CPP_HAVE_STRCASECMP */

/* Name of package */
#ifndef LOG4CPP_PACKAGE
#define LOG4CPP_PACKAGE  "log4cpp"
#endif

/* Version number of package */
#ifndef LOG4CPP_VERSION
#define LOG4CPP_VERSION  "1.0"
#endif

/* define if the compiler implements namespaces */
#ifndef LOG4CPP_HAVE_NAMESPACES
#define LOG4CPP_HAVE_NAMESPACES 1
#endif

/* define if the compiler has stringstream */
#ifndef LOG4CPP_HAVE_SSTREAM
#define LOG4CPP_HAVE_SSTREAM 1
#endif

#define LOG4CPP_HAS_WCHAR_T 0

/* define if the C library has snprintf */
#ifndef LOG4CPP_HAVE_SNPRINTF
#define LOG4CPP_HAVE_SNPRINTF 1
#endif

//#define LOG4CPP_HAVE_LOCALTIME_R 0

/* define to get around problems with ERROR in windows.h */
#ifndef LOG4CPP_FIX_ERROR_COLLISION
#define LOG4CPP_FIX_ERROR_COLLISION 1
#endif

/* use threads */
#ifndef LOG4CPP_HAVE_THREADING
#define LOG4CPP_HAVE_THREADING
#   if defined(LOG4CPP_STLPORT_AND_BOOST_BUILD)
#      define LOG4CPP_USE_BOOSTTHREADS
#   else
#      define LOG4CPP_USE_MSTHREADS
#   endif
#endif

#endif

