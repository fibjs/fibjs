/*
 *  osconfig.h
 *  Created on: 2012-3-2
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */



#ifndef OSCONFIG_H_
#define OSCONFIG_H_

# if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#  ifndef Windows
#   define Windows
#  endif
# endif

# if defined(linux) || defined(__linux) || defined(__linux__)
#  ifndef Linux
#   define Linux
#  endif
# endif

# if defined(__NetBSD__)
#  ifndef NetBSD
#   define NetBSD
#  endif
# endif

# if defined(__OpenBSD__)
#  ifndef OpenBSD
#   define OpenBSD
#  endif
# endif

# if (defined(__FreeBSD__) || defined(__DragonFly__) || \
		defined(__FreeBSD_kernel__)) && !defined(FREEBSD)
#  ifndef FreeBSD
#   define FreeBSD
#  endif
# endif

# if defined(sun) || defined(__sun)
#  ifndef Solaris
#   define Solaris
#  endif
# endif

# if defined(macosx) || (defined(__APPLE__) && defined(__MACH__))
#  ifndef MacOS
#   define MacOS
#  endif
# endif

# if defined(i386) || defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(x86)
#  ifndef I386
#   define I386
#  endif
# endif

# if defined(__amd64) || defined(__x86_64__) || defined(__ia64) || defined(__ia64__) || defined(_M_X64) || defined(_M_IA64)
#  ifndef x64
#   define x64
#  endif
# endif

# if defined(__AARCH64EL__)
#  define arm64
# endif

# if defined(__ARMEL__)
#  define arm
# endif

#endif /* OSCONFIG_H_ */
