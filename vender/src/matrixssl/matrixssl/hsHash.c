/*
 *	hsHash.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	"Native" handshake hash
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

#include "matrixsslApi.h"


#define FINISHED_LABEL_SIZE	15
#define LABEL_CLIENT		"client finished"
#define LABEL_SERVER		"server finished"
/******************************************************************************/
/*
	Initialize the SHA1 and MD5 hash contexts for the handshake messages
*/
int32 sslInitHSHash(ssl_t *ssl)
{

	psSha1Init(&ssl->sec.msgHashSha1);
	psMd5Init(&ssl->sec.msgHashMd5);
	return 0;
}

/******************************************************************************/
/*
	Add the given data to the running hash of the handshake messages
*/
int32 sslUpdateHSHash(ssl_t *ssl, unsigned char *in, uint32 len)
{

	psMd5Update(&ssl->sec.msgHashMd5, in, len);
	psSha1Update(&ssl->sec.msgHashSha1, in, len);

	return 0;
}


/******************************************************************************/
/*
	TLS handshake has computation
*/
static int32 tlsGenerateFinishedHash(ssl_t *ssl, psDigestContext_t *md5,
				psDigestContext_t *sha1, psDigestContext_t *sha256,
				psDigestContext_t *sha384, unsigned char *masterSecret,
				unsigned char *out, int32 sender)
{
	unsigned char	tmp[FINISHED_LABEL_SIZE + SHA384_HASH_SIZE];
	int32			tlsTmpSize;

	if (sender >= 0) {
		memcpy(tmp, (sender & SSL_FLAGS_SERVER) ? LABEL_SERVER : LABEL_CLIENT, 
			FINISHED_LABEL_SIZE);
		tlsTmpSize = FINISHED_LABEL_SIZE + SHA1_HASH_SIZE + MD5_HASH_SIZE;
		psMd5Final(md5, tmp + FINISHED_LABEL_SIZE);
		psSha1Final(sha1, tmp + FINISHED_LABEL_SIZE + MD5_HASH_SIZE);
		return prf(masterSecret, SSL_HS_MASTER_SIZE, tmp, tlsTmpSize, 
			out, TLS_HS_FINISHED_SIZE);
	} else {
		/* Overloading this function to handle the client auth needs of
			handshake hashing. */
/*
		The handshake snapshot for client authentication is simply the
		appended MD5 and SHA1 hashes
*/
		psMd5Final(md5, out);
		psSha1Final(sha1, out + MD5_HASH_SIZE);
		return MD5_HASH_SIZE + SHA1_HASH_SIZE;
	}
	return PS_FAILURE; /* Should not reach this */
}
/******************************************************************************/
/*
	Snapshot is called by the receiver of the finished message to produce
	a hash of the preceeding handshake messages for comparison to incoming
	message.
*/
int32 sslSnapshotHSHash(ssl_t *ssl, unsigned char *out, int32 senderFlag)
{
#ifdef USE_TLS
	psDigestContext_t	sha256, sha384;
#endif
	psDigestContext_t	md5, sha1;
	int32				len;
	

/*
	Use a backup of the message hash-to-date because we don't want
	to destroy the state of the handshaking until truly complete
*/
	md5 = ssl->sec.msgHashMd5;
	sha1 = ssl->sec.msgHashSha1;

#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		len = tlsGenerateFinishedHash(ssl, &md5, &sha1, &sha256, &sha384,
			ssl->sec.masterSecret, out, senderFlag);
	} else {
#endif /* USE_TLS */
#ifndef DISABLE_SSLV3
		len = sslGenerateFinishedHash(&md5, &sha1, ssl->sec.masterSecret,
			out, senderFlag);
#endif /* DISABLE_SSLV3 */			
#ifdef USE_TLS
	}
#endif /* USE_TLS */

	return len;
}

