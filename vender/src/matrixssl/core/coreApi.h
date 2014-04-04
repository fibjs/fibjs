/*
 *	coreApi.h
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Prototypes for the Matrix core public APIs
 */
/*
 *	Copyright (c) 2013 INSIDE Secure Corporation
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
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
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

#ifndef _h_PS_COREAPI
#define _h_PS_COREAPI

/******************************************************************************/
#include "coreConfig.h" /* Must be first included */
#include "osdep.h"
#include "list.h"
#include "psmalloc.h"

/******************************************************************************/
/*
	psCore return codes
*/
#define PS_CORE_IS_OPEN		1

/******************************************************************************/
/*
	Universal return codes
*/
#define PS_SUCCESS			0
#define PS_FAILURE			-1	

/*	NOTE: Failure return codes MUST be < 0 */
/*	NOTE: The range for core error codes should be between -2 and -29 */
#define PS_ARG_FAIL			-6	/* Failure due to bad function param */
#define PS_PLATFORM_FAIL	-7	/* Failure as a result of system call error */
#define PS_MEM_FAIL			-8	/* Failure to allocate requested memory */	
#define PS_LIMIT_FAIL		-9	/* Failure on sanity/limit tests */	
#define PS_UNSUPPORTED_FAIL	-10 /* Unimplemented feature error */	
#define PS_DISABLED_FEATURE_FAIL -11 /* Incorrect #define toggle for feature */
#define PS_PROTOCOL_FAIL	-12 /* A protocol error occurred */
#define PS_TIMEOUT_FAIL		-13 /* A timeout occurred and MAY be an error */
#define PS_INTERRUPT_FAIL	-14 /* An interrupt occurred and MAY be an error */
#define PS_PENDING			-15 /* In process. Not necessarily an error */ 
#define PS_EAGAIN			-16 /* Try again later. Not necessarily an error */ 

#define	PS_TRUE		1
#define	PS_FALSE 	0	

/******************************************************************************/
/* Public structures */
/******************************************************************************/
/*
	psBuf_t
	Empty buffer:
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|
	 ^
	 \end
	 \start
	 \buf
	 size = 16
	 len = (end - start) = 0

	Buffer with data:

     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	|.|.|a|b|c|d|e|f|g|h|i|j|.|.|.|.|
	 ^   ^                   ^
	 |   |                   \end
	 |   \start
	 \buf
	size = 16
	len = (end - start) = 10

	Read from start pointer
	Write to end pointer
	Free from buf pointer
*/
typedef struct {
	unsigned char	*buf;	/* Pointer to the start of the buffer */
	unsigned char	*start;	/* Pointer to start of valid data */
	unsigned char	*end;	/* Pointer to first byte of invalid data */
	int32			size;	/* Size of buffer in bytes */
} psBuf_t;

/******************************************************************************/
/*
	Public APIs
*/
/******************************************************************************/
PSPUBLIC int32		psCoreOpen(void);
PSPUBLIC void		psCoreClose(void);
PSPUBLIC void		psBurnStack(uint32 len);


/******************************************************************************/
/*
	Public interface to OS-dependant core functionality
	
	OS/osdep.c must implement the below functions
*/
PSPUBLIC int32		psGetEntropy(unsigned char *bytes, uint32 size);

PSPUBLIC int32		psGetTime(psTime_t *t);
PSPUBLIC int32		psDiffMsecs(psTime_t then, psTime_t now);
PSPUBLIC int32		psCompareTime(psTime_t a, psTime_t b);

#ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32		psGetFileBuf(psPool_t *pool, const char *fileName,
								 unsigned char **buf, int32 *bufLen);
#endif /* MATRIX_USE_FILE_SYSTEM */

#ifdef USE_MULTITHREADING
PSPUBLIC int32		psCreateMutex(psMutex_t *mutex);
PSPUBLIC int32		psLockMutex(psMutex_t *mutex);
PSPUBLIC int32		psUnlockMutex(psMutex_t *mutex);
PSPUBLIC void		psDestroyMutex(psMutex_t *mutex);
#endif /* USE_MULTITHREADING */

/******************************************************************************/
/*
	Internal list helpers
*/
extern int32 psParseList(psPool_t *pool, char *list, const char separator,
				psList_t **items);
extern void psFreeList(psList_t *list);

#endif /* _h_PS_COREAPI */
/******************************************************************************/

