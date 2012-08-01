/*
 *	osdep.c
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	WIN32 platform PScore 
 *		Windows XP Pro Service Pack 3 (Visual C++ 2008 Express Edition)
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

/******************************************************************************/
#ifdef WIN32 
/******************************************************************************/

#include <windows.h>
#include "../coreApi.h"

/******************************************************************************/
/*
    TIME FUNCTIONS
*/
/******************************************************************************/
static LARGE_INTEGER	hiresStart; /* zero-time */
static LARGE_INTEGER	hiresFreq; /* tics per second */
/*
    Module open and close
*/
int osdepTimeOpen(void)
{
	if (QueryPerformanceFrequency(&hiresFreq) != PS_TRUE) {
		return PS_FAILURE;
	}
	if (QueryPerformanceCounter(&hiresStart) != PS_TRUE) {
		return PS_FAILURE;
	}
	return PS_SUCCESS;
}

void osdepTimeClose(void)
{
}

/*
    PScore Public API implementations
*/
int32 psGetTime(psTime_t *t)
{
	psTime_t	lt;
    __int64		diff;
	int32			d;

	if (t == NULL) {
		QueryPerformanceCounter(&lt);
		diff = lt.QuadPart - hiresStart.QuadPart;
		d = (int32)((diff * 1000) / hiresFreq.QuadPart);
		return d;
	}

	QueryPerformanceCounter(t);
	diff = t->QuadPart - hiresStart.QuadPart;
	d = (int32)((diff * 1000) / hiresFreq.QuadPart);
	return d;
}

int32 psDiffMsecs(psTime_t then, psTime_t now) 
{
    __int64	diff;

	diff = now.QuadPart - then.QuadPart;
	return (int32)((diff*1000) / hiresFreq.QuadPart);
}

int32 psCompareTime(psTime_t a, psTime_t b)
{
	if (a.QuadPart <= b.QuadPart) {
		return 1;
	}
	return 0;
}


#ifdef USE_MULTITHREADING
/******************************************************************************/
/*
    MUTEX FUNCTIONS
*/
/******************************************************************************/
/*
    Module open and close
*/
int osdepMutexOpen(void)
{
	return PS_SUCCESS;
}

void osdepMutexClose(void)
{
}

/*
    PScore Public API implementations
*/
int32 psCreateMutex(psMutex_t *mutex)
{
	InitializeCriticalSection(mutex);
	return PS_SUCCESS;
}

int32 psLockMutex(psMutex_t *mutex)
{
	EnterCriticalSection(mutex);
	return PS_SUCCESS;
}

int32 psUnlockMutex(psMutex_t *mutex)
{
	LeaveCriticalSection(mutex);
	return PS_SUCCESS;
}

void psDestroyMutex(psMutex_t *mutex)
{
	DeleteCriticalSection(mutex);
}
#endif /* USE_MULTITHREADING */
/******************************************************************************/


/******************************************************************************/
/*
    ENTROPY FUNCTIONS
*/
/******************************************************************************/
static HCRYPTPROV		hProv;	/* Crypto context for random bytes */

int osdepEntropyOpen(void)
{
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 
			CRYPT_VERIFYCONTEXT))  {
		return PS_FAILURE;
	}
	return PS_SUCCESS;
}

void osdepEntropyClose(void)
{
	CryptReleaseContext(hProv, 0);
}

int32 psGetEntropy(unsigned char *bytes, uint32 size)
{
	if (CryptGenRandom(hProv, size, bytes)) {
		return size;
	}
	return PS_FAILURE;
}


/******************************************************************************/
/*
    TRACE FUNCTIONS
*/
/******************************************************************************/
int osdepTraceOpen(void)
{
	return PS_SUCCESS;
}

void osdepTraceClose(void)
{
}

void _psTrace(char *msg)
{
    printf(msg);
}

/* message should contain one %s, unless value is NULL */
void _psTraceStr(char *message, char *value)
{
    if (value) {
        printf(message, value);
    } else {
        printf(message);
    }
}

/* message should contain one %d */
void _psTraceInt(char *message, int32 value)
{
    printf(message, value);
}

/* message should contain one %p */
void _psTracePtr( char *message, void *value)
{
    printf(message, value);
}


#ifdef HALT_ON_PS_ERROR
/******************************************************************************/
/*
    system halt on psError when built HALT_ON_PS_ERROR 
*/
void osdepBreak(void)
{
     DebugBreak();
}
#endif /* HALT_ON_PS_ERROR */


#ifdef MATRIX_USE_FILE_SYSTEM
/******************************************************************************/
/*
    FILE ACCESS FUNCTION
*/
/******************************************************************************/
/*
    Memory info:
    Caller must free 'buf' parameter on success
    Callers do not need to free buf on function failure
*/
int32 psGetFileBuf(psPool_t *pool, const char *fileName, unsigned char **buf,
                int32 *bufLen)
{
	DWORD	dwAttributes;
	HANDLE	hFile;
	int32	size;
	DWORD	tmp = 0;

	*bufLen = 0;
	*buf = NULL;

	dwAttributes = GetFileAttributesA(fileName);
	if (dwAttributes != 0xFFFFFFFF && dwAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		psTraceStrCore("Unable to find %s\n", (char*)fileName);
        return PS_PLATFORM_FAIL;
	}

	if ((hFile = CreateFileA(fileName, GENERIC_READ,
			FILE_SHARE_READ && FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		psTraceStrCore("Unable to open %s\n", (char*)fileName);
        return PS_PLATFORM_FAIL;
	}
	
/*
 *	 Get the file size.
 */
	size = GetFileSize(hFile, NULL);

	*buf = psMalloc(pool, size + 1);
	if (*buf == NULL) {
		CloseHandle(hFile);
		return PS_MEM_FAIL;
	}
	memset(*buf, 0x0, size + 1);
	
	while (*bufLen < size) { 
		if (ReadFile(hFile, *buf + *bufLen,
				(size-*bufLen > 512 ? 512 : size-*bufLen), &tmp, NULL)
				== FALSE) {
			psFree(*buf);
			psTraceStrCore("Unable to read %s\n", (char*)fileName);
			CloseHandle(hFile);
			return PS_PLATFORM_FAIL;
		}
		*bufLen += (int32)tmp;
	}

	CloseHandle(hFile);
	return PS_SUCCESS;
}
#endif /* MATRIX_USE_FILE_SYSTEM */


/******************************************************************************/
#endif /* WIN32 */
/******************************************************************************/





