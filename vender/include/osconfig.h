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

# if defined(__amd64) || defined(__x86_64__) || defined(_M_X64)
#  ifndef x64
#   define x64
#  endif
# endif

# if defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#  ifndef IA64
#   define IA64
#  endif
# endif

# if defined(__AARCH64EL__)
#  ifndef arm64
#   define arm64
#  endif
# endif

# if defined(__ARMEL__)
#  ifndef arm
#   define arm
#  endif
# endif

#if defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \
      || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \
      || defined(_M_MPPC) || defined(_M_PPC)
#  if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
#    ifndef ppc64
#      define ppc64
#    endif
#  else
#    ifndef ppc
#      define ppc
#    endif
#  endif
#endif

#endif /* OSCONFIG_H_ */
