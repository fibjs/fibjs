/*
 *	corelib.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Open and Close APIs and utilities for Matrix core library
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

#include "coreApi.h"

static int32	psCoreIsOpen = PS_FALSE;

/******************************************************************************/
int32 psCoreOpen(void)
{
	if (psCoreIsOpen) {
		return PS_CORE_IS_OPEN;
	}	
		

	if (osdepTimeOpen() < 0) {
		psTraceCore("osdepTimeOpen failed\n");
		return PS_FAILURE;
	}
	if (osdepEntropyOpen() < 0) {
		psTraceCore("osdepEntropyOpen failed\n");
		osdepTimeClose();
		return PS_FAILURE;
	}
	
#ifdef USE_MULTITHREADING
	if (osdepMutexOpen() < 0) {
		psTraceCore("osdepMutexOpen failed\n");
		osdepEntropyClose();
		osdepTimeClose();
		return PS_FAILURE;
	}
#endif /* USE_MULTITHREADING */


	psCoreIsOpen = PS_TRUE;
	return PS_SUCCESS;
}

/******************************************************************************/
void psCoreClose(void)
{
	if (psCoreIsOpen != PS_FALSE) {
		psCoreIsOpen = PS_FALSE;
	
#ifdef USE_MULITHREADING
		osdepMutexClose();
#endif /* USE_MULITHREADING */

		osdepEntropyClose();

		osdepTimeClose();
	}
}

/******************************************************************************/
/*
	ERROR FUNCTIONS
	Tap into platform trace and break execution if DEBUG compile
	
	Modules should tie themselves to these low levels
	with compile-time defines
*/
void _psError(char *msg)
{
	_psTrace(msg);
	_psTrace("\n");
#ifdef HALT_ON_PS_ERROR	
	osdepBreak();
#endif	
} 
void _psErrorInt(char *msg, int32 val)
{
	_psTraceInt(msg, val);
	_psTrace("\n");
#ifdef HALT_ON_PS_ERROR		
	osdepBreak();
#endif
} 
void _psErrorStr(char *msg, char *val)
{
	_psTraceStr(msg, val);
	_psTrace("\n");
#ifdef HALT_ON_PS_ERROR		
	osdepBreak();
#endif	
} 

/*
	copy 'len' bytes from 'b' to 's', converting all to printable characters
*/
static void mem2str(char *s, unsigned char *b, uint32 len)
{
	for (; len > 0; len--) {
		if (*b > 31 && *b < 127) {
			*s = *b;
		} else {
			*s = '.';
		}
		b++;
		s++;
	}
}

void psTraceBytes(char *tag, unsigned char *p, int l)
{
	char	s[17];
	int		i;

	s[16] = '\0';
	_psTraceStr("psTraceBytes(%s, ", tag);
	_psTraceInt("%d);", l);
	for (i = 0; i < l; i++) {
		if (!(i & 0xF)) {
			if (i != 0) {
				mem2str(s, p - 16, 16);
				_psTraceStr("  %s", s);
			}
			_psTraceInt("\n0x%08x:", (int)p);
		}
		_psTraceInt("%02x ", *p++);
	}
	memset(s, 0x0, 16);
	i = l & 0xF;
	mem2str(s, p - i, (unsigned int)i);
	for (;i < 16; i++) {
		_psTrace("   ");
	}
	_psTraceStr("  %s", s);
	_psTrace("\n");
}

/******************************************************************************/
/*
	Creates a simple linked list from a given stream and separator char
	
	Memory info:
	Callers do not have to free 'items' on function failure.
*/
int32 psParseList(psPool_t *pool, char *list, const char separator,
		psList_t **items)
{
	psList_t	*litems, *start, *prev;
	uint32		itemLen, listLen;
	char		*tmp;

	*items = NULL;
	prev = NULL;
	
	listLen = (int32)strlen(list) + 1;
	if (listLen == 1) {
		return PS_ARG_FAIL;
	}
	start = litems = psMalloc(pool, sizeof(psList_t));
	if (litems == NULL) {
		return PS_MEM_FAIL;
	}
	memset(litems, 0, sizeof(psList_t));

	while (listLen > 0) {
		itemLen = 0;
		tmp = list;
		if (litems == NULL) {
			litems = psMalloc(pool, sizeof(psList_t));
			if (litems == NULL) {
				psFreeList(start);
				return PS_MEM_FAIL;
			}
			memset(litems, 0, sizeof(psList_t));
			prev->next = litems;
			
		}
		while (*list != separator && *list != '\0') {
			itemLen++;
			listLen--;
			list++;
		}
		litems->item = psMalloc(pool, itemLen + 1);
		if (litems->item == NULL) {
			psFreeList(start);
			return PS_MEM_FAIL;
		}
		litems->len = itemLen;
		memset(litems->item, 0x0, itemLen + 1);
		memcpy(litems->item, tmp, itemLen);
		list++;
		listLen--;
		prev = litems;
		litems = litems->next;
	}
	*items = start;
	return PS_SUCCESS;
}

void psFreeList(psList_t *list)
{
	psList_t	*next, *current;

	if (list == NULL) {
		return;
	}
	current = list;
	while (current) {
		next = current->next;
		if (current->item) {
			psFree(current->item);
		}
		psFree(current);
		current = next;
	}	
}

/******************************************************************************/
/*
	Clear the stack deeper than the caller to erase any potential secrets
	or keys.
*/
void psBurnStack(uint32 len)
{
	unsigned char buf[32];
	
	memset(buf, 0x0, sizeof(buf));
	if (len > (uint32)sizeof(buf)) {
		psBurnStack(len - sizeof(buf));
	}
}

/******************************************************************************/

