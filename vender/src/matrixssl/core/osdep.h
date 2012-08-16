/*
 *	osdep.h
 *	Operating System and Hardware Abstraction Layer
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 */
/*
 *	Copyright (c) AuthenTec, Inc. 2011-2012
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from AuthenTec at
 *	http://www.authentec.com/Products/EmbeddedSecurity/SecurityToolkits.aspx
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PS_PLATFORM
#define _h_PS_PLATFORM

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#else
#ifndef POSIX
#define POSIX
#endif
#endif

/******************************************************************************/
/*
    APIs that must be implemented on every platform
*/
extern int		osdepTraceOpen(void);
extern void		osdepTraceClose(void);
extern int		osdepTimeOpen(void);
extern void		osdepTimeClose(void);
extern int		osdepEntropyOpen(void);
extern void		osdepEntropyClose(void);
#ifdef HALT_ON_PS_ERROR
	extern void  osdepBreak(void);
#endif
#ifndef min
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif /* min */

/******************************************************************************/
/*
    If the Makefile specifies that MatrixSSL does not currently have
    a layer for the given OS, or the port is to "bare metal" hardware,
    do basic defines here and include externally provided file "matrixos.h".
    In addition, if building for such a platform, a C file defining the above
    functions must be linked with the final executable.
*/
#ifdef PS_UNSUPPORTED_OS
    #define PSPUBLIC extern
    #include "matrixos.h"
#else
/******************************************************************************/
/*
    Supported Platforms below. The implementations of the apis are in
    platform specific directories, such as core/POSIX and core/ECOS

    POSIX define is used for Linux and Mac OS X
*/
#include <stdio.h>
	
#ifndef POSIX
	#if defined(LINUX) || defined(OSX)
	#define POSIX
	#endif
#endif

#ifdef POSIX
	#include <stdint.h>
	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int16_t int16;
	typedef uint16_t uint16;
	#ifdef HAVE_NATIVE_INT64 
	typedef int64_t int64;
	typedef uint64_t uint64;
	#endif
#endif /* POSIX */
#ifdef WIN32
	#include <windows.h>
	typedef signed long int32;
	typedef unsigned long uint32;
	typedef signed short int16;
	typedef unsigned short uint16;
	#ifdef HAVE_NATIVE_INT64 
	typedef unsigned long long	uint64;
	typedef signed long long	int64;
	#endif
#endif /* WIN32 */

/******************************************************************************/
/*
	Hardware Abstraction Layer
*/
#define halAlert()

/******************************************************************************/
/*
	OS-specific psTime_t types
	
	Make psTime_t an opaque time value.
*/
#ifdef WIN32
	typedef LARGE_INTEGER psTime_t;
#endif
#ifdef VXWORKS
	typedef struct {
		long sec;
		long usec;
	} psTime_t;
#endif

/* #define USE_HIGHRES_TIME */
#ifdef POSIX
	#include <sys/time.h>
	#include <time.h>
	typedef struct timeval psTime_t;
#endif

/******************************************************************************/
/*
	Export or import functions for Windows DLLs	
*/
//#ifdef WIN32
//	#ifndef _USRDLL
//	#define PSPUBLIC extern __declspec(dllimport)
//	#else
//	#define PSPUBLIC extern __declspec(dllexport)
//	#endif
//#else
	#define PSPUBLIC extern
//#endif /* !WIN */

/******************************************************************************/
/*
	Raw trace and error
*/
PSPUBLIC void _psTrace(char *msg);
PSPUBLIC void _psTraceInt(char *msg, int32 val);
PSPUBLIC void _psTraceStr(char *msg, char *val);
PSPUBLIC void _psTracePtr(char *message, void *value);
PSPUBLIC void psTraceBytes(char *tag, unsigned char *p, int l);

PSPUBLIC void _psError(char *msg);
PSPUBLIC void _psErrorInt(char *msg, int32 val);
PSPUBLIC void _psErrorStr(char *msg, char *val);

/******************************************************************************/
/*
	Core trace
*/
#ifndef USE_CORE_TRACE
#define psTraceCore(x) 
#define psTraceStrCore(x, y) 
#define psTraceIntCore(x, y)
#define psTracePtrCore(x, y)
#else
#define psTraceCore(x) _psTrace(x)
#define psTraceStrCore(x, y) _psTraceStr(x, y)
#define psTraceIntCore(x, y) _psTraceInt(x, y)
#define psTracePtrCore(x, y) _psTracePtr(x, y)
#endif /* USE_CORE_TRACE */

/******************************************************************************/
/*
	HALT_ON_PS_ERROR define at compile-time determines whether to halt on
	psAssert and psError calls
*/ 
#define psAssert(C)  if (C) ; else \
{halAlert();_psTraceStr("psAssert %s", __FILE__);_psTraceInt(":%d ", __LINE__);\
_psError(#C);} 

#define psError(a) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psError(a);
 
#define psErrorStr(a,b) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psErrorStr(a,b)
 
#define psErrorInt(a,b) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psErrorInt(a,b)

/******************************************************************************/
/*
	OS specific file system apis
*/
#ifdef MATRIX_USE_FILE_SYSTEM
	#ifdef POSIX
	#include <sys/stat.h>
	#endif /* POSIX */
#endif /* MATRIX_USE_FILE_SYSTEM */

#ifdef USE_MULTITHREADING
/******************************************************************************/
/*
	Defines to make library multithreading safe
*/
extern int32 osdepMutexOpen(void);
extern int32 osdepMutexClose(void);

#ifdef WIN32
	typedef CRITICAL_SECTION psMutex_t;
#elif POSIX
	#include <pthread.h>
	#include <string.h>
	typedef pthread_mutex_t psMutex_t;
#elif VXWORKS
	#include "semLib.h"
	typedef SEM_ID	psMutex_t; 
#else
	#error psMutex_t must be defined
#endif /* OS specific mutex */
#endif /* USE_MULTITHREADING */

/******************************************************************************/

#endif /* !PS_UNSUPPORTED_OS */
#endif /* _h_PS_PLATFORM */

