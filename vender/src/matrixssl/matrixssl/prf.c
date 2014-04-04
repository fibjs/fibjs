/*
 *	prf.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	"Native" Pseudo Random Function
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

#include "matrixsslApi.h"

#ifdef USE_TLS
/******************************************************************************/
/*
	MD5 portions of the prf
*/
static int32 pMd5(unsigned char *key, uint32 keyLen, 
				unsigned char *text, uint32 textLen,
				unsigned char *out, uint32 outLen)
{
	psHmacContext_t	ctx;
	unsigned char	a[MD5_HASH_SIZE];
	unsigned char	mac[MD5_HASH_SIZE];
	unsigned char	hmacKey[MD5_HASH_SIZE];
	int32			i, keyIter = 1;
	uint32			hmacKeyLen;

	while ((uint32)(MD5_HASH_SIZE * keyIter) < outLen) {
		keyIter++;
	}
	psHmacMd5(key, keyLen, text, textLen, a, hmacKey, &hmacKeyLen);
	if (hmacKeyLen != keyLen) {
/*
		Support for keys larger than 64 bytes.  Must take the hash of
		the original key in these cases which is indicated by different
		outgoing values from the passed in key and keyLen values
*/
		psAssert(keyLen > 64);
		key = hmacKey;
		keyLen = hmacKeyLen;
	}
	for (i = 0; i < keyIter; i++) {
		psHmacMd5Init(&ctx, key, keyLen);
		psHmacMd5Update(&ctx, a, MD5_HASH_SIZE);
		psHmacMd5Update(&ctx, text, textLen);
		psHmacMd5Final(&ctx, mac);
		if (i == keyIter - 1) {
			memcpy(out + (MD5_HASH_SIZE*i), mac, outLen - (MD5_HASH_SIZE*i));
		} else {
			memcpy(out + (MD5_HASH_SIZE * i), mac, MD5_HASH_SIZE);
			psHmacMd5(key, keyLen, a, MD5_HASH_SIZE, a, hmacKey, &hmacKeyLen);
		}
	}
	return 0;
}

/******************************************************************************/
/*
	SHA1 portion of the prf
*/
static int32 pSha1(unsigned char *key, uint32 keyLen, 
					unsigned char *text, uint32 textLen,
					unsigned char *out, uint32 outLen)
{
	psHmacContext_t	ctx;
	unsigned char		a[SHA1_HASH_SIZE];
	unsigned char		mac[SHA1_HASH_SIZE];
	unsigned char		hmacKey[SHA1_HASH_SIZE];
	int32				i, keyIter = 1;
	uint32				hmacKeyLen;

	while ((uint32)(SHA1_HASH_SIZE * keyIter) < outLen) {
		keyIter++;
	}
	psHmacSha1(key, keyLen, text, textLen, a, hmacKey, &hmacKeyLen);
	if (hmacKeyLen != keyLen) {
/*
		Support for keys larger than 64 bytes.  Must take the hash of
		the original key in these cases which is indicated by different
		outgoing values from the passed in key and keyLen values
*/
		psAssert(keyLen > 64);
		key = hmacKey;
		keyLen = hmacKeyLen;
	}
	for (i = 0; i < keyIter; i++) {
		psHmacSha1Init(&ctx, key, keyLen);
		psHmacSha1Update(&ctx, a, SHA1_HASH_SIZE);
		psHmacSha1Update(&ctx, text, textLen);
		psHmacSha1Final(&ctx, mac);
		if (i == keyIter - 1) {
			memcpy(out + (SHA1_HASH_SIZE * i), mac,
				outLen - (SHA1_HASH_SIZE * i));
		} else {
			memcpy(out + (SHA1_HASH_SIZE * i), mac, SHA1_HASH_SIZE);
			psHmacSha1(key, keyLen, a, SHA1_HASH_SIZE, a, hmacKey,
				&hmacKeyLen);
		}
	}
	return 0;
}


/******************************************************************************/
/*
	Psuedo-random function.  TLS uses this for key generation and hashing
*/
int32 prf(unsigned char *sec, uint32 secLen, unsigned char *seed,
			   uint32 seedLen, unsigned char *out, uint32 outLen)
{
	unsigned char	*s1, *s2;
	unsigned char	md5out[SSL_MAX_KEY_BLOCK_SIZE];
	unsigned char	sha1out[SSL_MAX_KEY_BLOCK_SIZE];
	uint32			sLen, i;

	psAssert(outLen <= SSL_MAX_KEY_BLOCK_SIZE);

	sLen = (secLen / 2) + (secLen % 2);
	s1 = sec;
	s2 = (sec + sLen) - (secLen % 2);
	pMd5(s1, sLen, seed, seedLen, md5out, outLen);
	pSha1(s2, sLen, seed, seedLen, sha1out, outLen);
	for (i = 0; i < outLen; i++) {
		out[i] = md5out[i] ^ sha1out[i];
	}
	return outLen;
}

#endif /* USE_TLS */
/******************************************************************************/

