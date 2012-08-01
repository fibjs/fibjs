/*
 *	pubkey.c
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

#include "../cryptoApi.h"

/******************************************************************************/
/*
	Allocate a new psPubKey_t and memset empty 
*/
psPubKey_t * psNewPubKey(psPool_t *pool) {

	psPubKey_t *ret;

	ret = psMalloc(pool, sizeof(psPubKey_t));

	if (ret == NULL) {
		psError("Memory allocation error in psNewPubKey\n");
		return NULL;
	}
	memset(ret, 0x0, sizeof(psPubKey_t));
	ret->key = psMalloc(pool, sizeof(pubKeyUnion_t));
	if (ret->key == NULL) {
		psFree(ret);
		psError("Memory allocation error in psNewPubKey\n");
		return NULL;
	}
	memset(ret->key, 0x0, sizeof(pubKeyUnion_t));
	return ret;
}

/******************************************************************************/
/*
	
*/
void psFreePubKey(psPubKey_t *key)
{
	if (key == NULL) {
		return;
	}
	if (key->type == PS_RSA) {
#ifdef USE_RSA
		psRsaFreeKey((psRsaKey_t*)key->key);
#else
		psFree(key->key);
#endif		

	} else {
/*
		If type not found, assume an empty key type
*/
		psFree(key->key);
	}
	psFree(key);
}

/******************************************************************************/
