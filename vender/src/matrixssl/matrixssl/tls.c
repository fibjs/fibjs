/*
 *	tls.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	TLS (SSLv3.1+) specific code
 *	http://www.faqs.org/rfcs/rfc2246.html
 *	Primarily dealing with secret generation, message authentication codes
 *	and handshake hashing.
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

/******************************************************************************/
#ifdef USE_TLS
/******************************************************************************/

#define LABEL_SIZE			13
#define LABEL_MASTERSEC		"master secret"
#define LABEL_KEY_BLOCK		"key expansion"

/******************************************************************************/
/*
 *	Generates all key material.
 */
int32 tlsDeriveKeys(ssl_t *ssl)
{
	unsigned char	msSeed[SSL_HS_RANDOM_SIZE * 2 + LABEL_SIZE];
	uint32			reqKeyLen;
	
/*
	If this session is resumed, we want to reuse the master secret to 
	regenerate the key block with the new random values.
*/
	if (ssl->flags & SSL_FLAGS_RESUMED) {
		goto skipPremaster;
	}
/*
	master_secret = PRF(pre_master_secret, "master secret",
		client_random + server_random);
*/
	memcpy(msSeed, LABEL_MASTERSEC, LABEL_SIZE); 
	memcpy(msSeed + LABEL_SIZE, ssl->sec.clientRandom,
		SSL_HS_RANDOM_SIZE);
	memcpy(msSeed + LABEL_SIZE + SSL_HS_RANDOM_SIZE,
		ssl->sec.serverRandom, SSL_HS_RANDOM_SIZE);
			
	prf(ssl->sec.premaster, ssl->sec.premasterSize, msSeed,
		(SSL_HS_RANDOM_SIZE * 2) + LABEL_SIZE, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);

/*
	 premaster is now allocated for DH reasons.  Can free here
*/
	psFree(ssl->sec.premaster);
	ssl->sec.premaster = NULL;
	ssl->sec.premasterSize = 0;

skipPremaster:
	memcpy(msSeed, LABEL_KEY_BLOCK, LABEL_SIZE); 
	memcpy(msSeed + LABEL_SIZE, ssl->sec.serverRandom,
		SSL_HS_RANDOM_SIZE);
	memcpy(msSeed + LABEL_SIZE + SSL_HS_RANDOM_SIZE,
		ssl->sec.clientRandom, SSL_HS_RANDOM_SIZE);
/*
	We must generate enough key material to fill the various keys
*/
	reqKeyLen = 2 * ssl->cipher->macSize + 
				2 * ssl->cipher->keySize + 
				2 * ssl->cipher->ivSize;

	prf(ssl->sec.masterSecret, SSL_HS_MASTER_SIZE, msSeed,
		(SSL_HS_RANDOM_SIZE * 2) + LABEL_SIZE, ssl->sec.keyBlock,
		reqKeyLen);
	if (ssl->flags & SSL_FLAGS_SERVER) {
		ssl->sec.rMACptr = ssl->sec.keyBlock;
		ssl->sec.wMACptr = ssl->sec.rMACptr + ssl->cipher->macSize;
		ssl->sec.rKeyptr = ssl->sec.wMACptr + ssl->cipher->macSize;
		ssl->sec.wKeyptr = ssl->sec.rKeyptr + ssl->cipher->keySize;
		ssl->sec.rIVptr = ssl->sec.wKeyptr + ssl->cipher->keySize;
		ssl->sec.wIVptr = ssl->sec.rIVptr + ssl->cipher->ivSize;
	} else {
		ssl->sec.wMACptr = ssl->sec.keyBlock;
		ssl->sec.rMACptr = ssl->sec.wMACptr + ssl->cipher->macSize;
		ssl->sec.wKeyptr = ssl->sec.rMACptr + ssl->cipher->macSize;
		ssl->sec.rKeyptr = ssl->sec.wKeyptr + ssl->cipher->keySize;
		ssl->sec.wIVptr = ssl->sec.rKeyptr + ssl->cipher->keySize;
		ssl->sec.rIVptr = ssl->sec.wIVptr + ssl->cipher->ivSize;
	}

	return SSL_HS_MASTER_SIZE;
}

#ifdef USE_SHA_MAC
/******************************************************************************/
/*
	TLS sha1 HMAC generate/verify
*/
int32 tlsHMACSha1(ssl_t *ssl, int32 mode, unsigned char type,	
			unsigned char *data, uint32 len, unsigned char *mac)
{	
	psHmacContext_t		ctx;
	unsigned char		*key, *seq;
	unsigned char		majVer, minVer, tmp[5];
	int32				i;
		
	majVer = ssl->majVer;
	minVer = ssl->minVer;
	
	if (mode == HMAC_CREATE) {
		key = ssl->sec.writeMAC;
		seq = ssl->sec.seq;
	} else { /* HMAC_VERIFY */
		key = ssl->sec.readMAC;
		seq = ssl->sec.remSeq;
	}
	
	psHmacSha1Init(&ctx, key, SHA1_HASH_SIZE);
		psHmacSha1Update(&ctx, seq, 8);
		for (i = 7; i >= 0; i--) {
			seq[i]++;
			if (seq[i] != 0) {
				break; 
			}
		}
	
	tmp[0] = type;
	tmp[1] = majVer;
	tmp[2] = minVer;
	tmp[3] = (len & 0xFF00) >> 8;
	tmp[4] = len & 0xFF;
	psHmacSha1Update(&ctx, tmp, 5);
	psHmacSha1Update(&ctx, data, len);
	return psHmacSha1Final(&ctx, mac);
}

#endif /* USE_SHA_MAC */

#ifdef USE_MD5_MAC
/******************************************************************************/
/*
	TLS MD5 HMAC generate/verify
*/
int32 tlsHMACMd5(ssl_t *ssl, int32 mode, unsigned char type,	
				 unsigned char *data, uint32 len, unsigned char *mac)
{
	psHmacContext_t		ctx;
	unsigned char		*key, *seq;
	unsigned char		majVer, minVer, tmp[5];
	int32				i;

	majVer = ssl->majVer;
	minVer = ssl->minVer;
	
	if (mode == HMAC_CREATE) {
		key = ssl->sec.writeMAC;
		seq = ssl->sec.seq;
	} else { /* HMAC_VERIFY */
		key = ssl->sec.readMAC;
		seq = ssl->sec.remSeq;
	}
	
	psHmacMd5Init(&ctx, key, MD5_HASH_SIZE);
		psHmacMd5Update(&ctx, seq, 8);
		for (i = 7; i >= 0; i--) {
			seq[i]++;
			if (seq[i] != 0) {
				break; 
			}
		}

	tmp[0] = type;
	tmp[1] = majVer;
	tmp[2] = minVer;
	tmp[3] = (len & 0xFF00) >> 8;
	tmp[4] = len & 0xFF;
	psHmacMd5Update(&ctx, tmp, 5);
	psHmacMd5Update(&ctx, data, len);
	return psHmacMd5Final(&ctx, mac);
}
#endif /* USE_MD5_MAC */


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

/******************************************************************************/
#endif /* USE_TLS */


#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Allocate a tlsExtenstion_t structure
*/
int32 matrixSslNewHelloExtension(tlsExtension_t **extension)
{
	psPool_t		*pool = NULL;
	tlsExtension_t	*ext;
	
   
    ext = psMalloc(pool, sizeof(tlsExtension_t));
    if (ext == NULL) {
        return PS_MEM_FAIL;
    }
    memset(ext, 0x0, sizeof(tlsExtension_t));
    ext->pool = pool;
   
    *extension = ext;
    return PS_SUCCESS;
}

/******************************************************************************/
/*
	Free a tlsExtenstion_t structure and any extensions that have been loaded
*/
void matrixSslDeleteHelloExtension(tlsExtension_t *extension)
{
	tlsExtension_t	*next, *ext;
	
	if (extension == NULL) {
		return;
	}
	ext = extension;
/*
	Free first one
*/
	if (ext->extData) {
		psFree(ext->extData);
	}
	next = ext->next;
	psFree(ext);
/*
	Free others
*/
	while (next) {
		ext = next;
		next = ext->next;
		if (ext->extData) {
			psFree(ext->extData);
		}
		psFree(ext);
	}
	
	return; 
}

/*****************************************************************************/
/*
	Add an outgoing CLIENT_HELLO extension to a tlsExtension_t structure
	that was previously allocated with matrixSslNewHelloExtension
*/
int32 matrixSslLoadHelloExtension(tlsExtension_t *ext,
			unsigned char *extension, uint32 length, uint32 extType)
{
	tlsExtension_t	*current, *new;

	if (ext == NULL || (length > 0 && extension == NULL)) {
		return PS_ARG_FAIL;
	}
/*
	Find first empty spot in ext.  This is determined by extLen since even
	an empty extension will have a length of 1 for the 0
*/
	current = ext;
	while (current->extLen != 0) {
		if (current->next != NULL) {
			current = current->next;
			continue;
		}
		new = psMalloc(ext->pool, sizeof(tlsExtension_t));
		if (new == NULL) {
			return PS_MEM_FAIL;
		}
		memset(new, 0, sizeof(tlsExtension_t));
		new->pool = ext->pool;
		current->next = new;
		current = new;
	}
/*
	Supports an empty extension which is really a one byte 00:
		ff 01 00 01 00  (two byte type, two byte len, one byte 00)
		
	This will either be passed in as a NULL 'extension' with a 0 length - OR -
	A pointer to a one byte 0x0 and a length of 1.  In either case, the
	structure will identify the ext with a length of 1 and a NULL data ptr
*/
	current->extType = extType;
	if (length > 0) {
		current->extLen = length;
		if (length == 1 && extension[0] == '\0') {
			current->extLen = 1;
		} else {
			current->extData = psMalloc(ext->pool, length);
			if (current->extData == NULL) {
				return PS_MEM_FAIL;
			}
			memcpy(current->extData, extension, length);
		}
	} else if (length == 0) {
		current->extLen = 1;
	}

	return PS_SUCCESS;
}
#endif /* USE_CLIENT_SIDE_SSL */


/******************************************************************************/

