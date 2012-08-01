/*
 *	nativCrypto.c
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	TLS (SSLv3.1+) specific code written to the original Matrix crypto lib
 *	
 */
/*
 *	Copyright (c) AuthenTec, Inc. 2012
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

#include "matrixsslApi.h"


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
	Snapshot is called by the receiver of the finished message to produce
	a hash of the preceeding handshake messages for comparison to incoming
	message.
*/
int32 sslSnapshotHSHash(ssl_t *ssl, unsigned char *out, int32 senderFlag)
{
#ifdef USE_TLS
	psDigestContext_t	sha256;
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
		len = tlsGenerateFinishedHash(ssl, &md5, &sha1, &sha256,
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

int32 sslCreateKeys(ssl_t *ssl)
{
#ifdef USE_TLS
		if (ssl->flags & SSL_FLAGS_TLS) {		
			return tlsDeriveKeys(ssl);		
		} else {
#ifndef DISABLE_SSLV3		
			return sslDeriveKeys(ssl);
#else
			return PS_ARG_FAIL;
#endif /* DISABLE_SSLV3 */			
		}
#else /* SSLv3 only below */
#ifndef DISABLE_SSLV3
		return sslDeriveKeys(ssl);
#endif /* DISABLE_SSLV3 */		
#endif /* USE_TLS */
}

/******************************************************************************/
/*
	Cipher suites are chosen before they are activated with the 
	ChangeCipherSuite message.  Additionally, the read and write cipher suites
	are activated at different times in the handshake process.  The following
	APIs activate the selected cipher suite callback functions.
*/
int32 sslActivateReadCipher(ssl_t *ssl)
{
	ssl->decrypt = ssl->cipher->decrypt;
	ssl->verifyMac = ssl->cipher->verifyMac;
	ssl->deMacSize = ssl->cipher->macSize;
	ssl->deBlockSize = ssl->cipher->blockSize;
	ssl->deIvSize = ssl->cipher->ivSize;
/*
	Reset the expected incoming sequence number for the new suite
*/
	memset(ssl->sec.remSeq, 0x0, sizeof(ssl->sec.remSeq));

	if (ssl->cipher->ident != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_READ_SECURE;
		
/*
		Copy the newly activated read keys into the live buffers
*/
		memcpy(ssl->sec.readMAC, ssl->sec.rMACptr, ssl->deMacSize);
		memcpy(ssl->sec.readKey, ssl->sec.rKeyptr, ssl->cipher->keySize);
		memcpy(ssl->sec.readIV, ssl->sec.rIVptr, ssl->cipher->ivSize);
/*
		set up decrypt contexts
*/
		if (ssl->cipher->init) {
			if (ssl->cipher->init(&(ssl->sec), INIT_DECRYPT_CIPHER,
					ssl->cipher->keySize) < 0) {
				psTraceInfo("Unable to initialize read cipher suite\n");
				return PS_FAILURE;
			}
		}
	}
	return PS_SUCCESS;
}

int32 sslActivateWriteCipher(ssl_t *ssl)
{

	ssl->encrypt = ssl->cipher->encrypt;
	ssl->generateMac = ssl->cipher->generateMac;
	ssl->enMacSize = ssl->cipher->macSize;
	ssl->enBlockSize = ssl->cipher->blockSize;
	ssl->enIvSize = ssl->cipher->ivSize;
/*
	Reset the outgoing sequence number for the new suite
*/
	memset(ssl->sec.seq, 0x0, sizeof(ssl->sec.seq));
	if (ssl->cipher->ident != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_WRITE_SECURE;

		
/*
		Copy the newly activated write keys into the live buffers
*/
		memcpy(ssl->sec.writeMAC, ssl->sec.wMACptr, ssl->enMacSize);
		memcpy(ssl->sec.writeKey, ssl->sec.wKeyptr, ssl->cipher->keySize);
		memcpy(ssl->sec.writeIV, ssl->sec.wIVptr, ssl->cipher->ivSize);
/*
		set up encrypt contexts
 */
		if (ssl->cipher->init) {
			if (ssl->cipher->init(&(ssl->sec), INIT_ENCRYPT_CIPHER,
					ssl->cipher->keySize) < 0) {
				psTraceInfo("Unable to init write cipher suite\n");
				return PS_FAILURE;
			}
		}
	}
	return PS_SUCCESS;
}


