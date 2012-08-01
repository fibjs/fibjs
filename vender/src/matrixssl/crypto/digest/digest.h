/*
 *	digest.h
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	Header for internal symmetric key cryptography support
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

#ifndef _h_PS_DIGEST
#define _h_PS_DIGEST
/******************************************************************************/
#ifdef USE_SHA1
#define SHA1_HASH_SIZE 20
struct sha1_state {
#ifdef HAVE_NATIVE_INT64
	uint64		length;
#else
	uint32		lengthHi;
	uint32		lengthLo;
#endif /* HAVE_NATIVE_INT64 */
	uint32		state[5], curlen;
	unsigned char	buf[64];
};
#endif /* USE_SHA1 */

#define SHA256_HASH_SIZE 32 

#ifdef USE_MD5
#define MD5_HASH_SIZE 16 
struct md5_state {
#ifdef HAVE_NATIVE_INT64
    uint64 length;
#else
    uint32 lengthHi;
    uint32 lengthLo;
#endif /* HAVE_NATIVE_INT64 */
    uint32 state[4], curlen;
    unsigned char buf[64];
};
#endif /* USE_MD5 */





#define SHA384_HASH_SIZE 48 


#define MAX_HASH_SIZE	SHA1_HASH_SIZE

/******************************************************************************/
typedef union {
#ifdef USE_SHA1
	struct sha1_state	sha1;
#endif /* USE_SHA1 */

#ifdef USE_MD5
	struct md5_state	md5;
#endif /* USE_MD5 */




} psDigestContext_t;


/******************************************************************************/
#ifdef USE_HMAC
/******************************************************************************/
typedef struct {
	unsigned char	pad[64];
	union {
		psDigestContext_t	md5;
		psDigestContext_t	sha1;
		psDigestContext_t	sha256;
		psDigestContext_t	sha512;
	} u;
} psHmacContext_t;
#endif /* USE_HMAC */
/******************************************************************************/

#endif /* _h_PS_DIGEST */
/******************************************************************************/

