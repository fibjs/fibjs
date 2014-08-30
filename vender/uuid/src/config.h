/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

#include <exlib/include/osconfig.h>

/* Predefined possible va_copy() implementation (id: ASP) */
#define __VA_COPY_USE_ASP(d, s) do { *(d) = *(s); } while (0)

/* Predefined possible va_copy() implementation (id: ASS) */
#define __VA_COPY_USE_ASS(d, s) do { (d) = (s); } while (0)

/* Predefined possible va_copy() implementation (id: C99) */
#define __VA_COPY_USE_C99(d, s) va_copy((d), (s))

/* Predefined possible va_copy() implementation (id: CPP) */
#define __VA_COPY_USE_CPP(d, s) memcpy((void *)(d), (void *)(s), sizeof(*(s)))

/* Predefined possible va_copy() implementation (id: CPS) */
#define __VA_COPY_USE_CPS(d, s) memcpy((void *)&(d), (void *)&(s), sizeof((s)))

/* Predefined possible va_copy() implementation (id: GCB) */
#define __VA_COPY_USE_GCB(d, s) __builtin_va_copy((d), (s))

/* Predefined possible va_copy() implementation (id: GCH) */
#define __VA_COPY_USE_GCH(d, s) __va_copy((d), (s))

/* Predefined possible va_copy() implementation (id: GCM) */
#define __VA_COPY_USE_GCM(d, s) VA_COPY((d), (s))

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the `clock_gettime' function. */
/* #undef HAVE_CLOCK_GETTIME */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <dmalloc.h> header file. */
/* #undef HAVE_DMALLOC_H */

/* Define to 1 if you have the `getifaddrs' function. */
#define HAVE_GETIFADDRS 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <ifaddrs.h> header file. */
#define HAVE_IFADDRS_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `dmalloc' library (-ldmalloc). */
/* #undef HAVE_LIBDMALLOC */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if the system has the type `long double'. */
#define HAVE_LONG_DOUBLE 1

/* Define to 1 if the system has the type `long long'. */
#define HAVE_LONG_LONG 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <net/if_arp.h> header file. */
#define HAVE_NET_IF_ARP_H 1

/* Define to 1 if you have the <net/if.h> header file. */
#define HAVE_NET_IF_H 1

/* Define to 1 if you have the `Sleep' function. */
/* #undef HAVE_SLEEP */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* define if exists "struct timeval" */
#define HAVE_STRUCT_TIMEVAL 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

#ifdef MacOS

/* Define to 1 if you have the `nsl' library (-lnsl). */
//#define HAVE_LIBNSL 1

/* Define to 1 if you have the <net/if_dl.h> header file. */
#define HAVE_NET_IF_DL_H 1

/* Define to 1 if you have the <sys/sockio.h> header file. */
#define HAVE_SYS_SOCKIO_H 1

#else

/* Define to 1 if you have the `nsl' library (-lnsl). */
#define HAVE_LIBNSL 1

/* Define to 1 if you have the <net/if_dl.h> header file. */
//#define HAVE_NET_IF_DL_H 1

/* Define to 1 if you have the <sys/sockio.h> header file. */
//#define HAVE_SYS_SOCKIO_H 1

#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if va_copy() macro exists (and no fallback implementation is
   required) */
#define HAVE_VA_COPY 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `unsigned char', as computed by sizeof. */
#define SIZEOF_UNSIGNED_CHAR 1

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* The size of `unsigned long long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* The size of `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* whether to build C++ bindings to C API */
/* #undef WITH_CXX */

/* whether to build DCE 1.1 backward compatibility API */
/* #undef WITH_DCE */

/* define if building with Dmalloc */
/* #undef WITH_DMALLOC */

/* whether to build Perl bindings to C API */
/* #undef WITH_PERL */

/* whether to build Perl compatibility API */
/* #undef WITH_PERL_COMPAT */

/* whether to build PostgreSQL bindings to C API */
/* #undef WITH_PGSQL */

/* whether to build PHP bindings to C API */
/* #undef WITH_PHP */

/* Optional va_copy() implementation activation */
#ifndef HAVE_VA_COPY
#define va_copy(d, s) __VA_COPY_USE(d, s)
#endif


/* Define to id of used va_copy() implementation */
#define __VA_COPY_USE __VA_COPY_USE_C99
