/*
 *	cipherSuite.c
 *	Release $Name: MATRIXSSL-3-4-2-OPEN $
 *
 *	Wrappers for the various cipher suites.
 *	Enable specific suites at compile time in matrixsslConfig.h
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
/*	Symmetric cipher initializtion wrappers for cipher suites */
/******************************************************************************/
/*
	SSL_NULL_WITH_NULL_NULL cipher functions
	Used in handshaking before SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC message
*/
static int32 csNullInit(sslSec_t *sec, int32 type, uint32 keysize)
{
	return 0;
}

/******************************************************************************/
#ifdef USE_ARC4_CIPHER_SUITE
/******************************************************************************/
static int32 csArc4Init(sslSec_t *sec, int32 type, uint32 keysize)
{
	if (type == INIT_ENCRYPT_CIPHER) {
		memset(&(sec->encryptCtx), 0, sizeof(psRc4Key_t));
		psArc4Init(&(sec->encryptCtx), sec->writeKey, keysize);
	} else {
		memset(&(sec->decryptCtx), 0, sizeof(psRc4Key_t));
		psArc4Init(&(sec->decryptCtx), sec->readKey, keysize);
	}
	return PS_SUCCESS;
}
int32 csArc4Encrypt(void *ssl, unsigned char *pt,
					 unsigned char *ct, uint32 len)
{
	ssl_t	*lssl = ssl;
	psCipherContext_t *ctx = (psCipherContext_t*)&lssl->sec.encryptCtx;
	return psArc4(ctx, pt, ct, len);	
}
int32 csArc4Decrypt(void *ssl, unsigned char *ct,
					 unsigned char *pt, uint32 len)
{
	ssl_t	*lssl = ssl;
	psCipherContext_t *ctx = (psCipherContext_t*)&lssl->sec.decryptCtx;
	return psArc4(ctx, ct, pt, len);	
}

#endif /* USE_ARC4_CIPHER_SUITE */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_3DES_CIPHER_SUITE
/******************************************************************************/
static int32 csDes3Init(sslSec_t *sec, int32 type, uint32 keysize)
{
	int32	err;
	
	psAssert(keysize == DES3_KEY_LEN);
	
	if (type == INIT_ENCRYPT_CIPHER) {
		memset(&(sec->encryptCtx), 0, sizeof(des3_CBC));	
		if ((err = psDes3Init(&(sec->encryptCtx), sec->writeIV, sec->writeKey, 
							  DES3_KEY_LEN)) < 0) {
			return err;
		}
	} else {
		memset(&(sec->decryptCtx), 0, sizeof(des3_CBC));
		if ((err = psDes3Init(&(sec->decryptCtx), sec->readIV, sec->readKey, 
							  DES3_KEY_LEN)) < 0) {
			return err;
		}
	}
	return PS_SUCCESS;
}

int32 csDes3Encrypt(void *ssl, unsigned char *pt,
					 unsigned char *ct, uint32 len)
{
	ssl_t	*lssl = ssl;
	psCipherContext_t *ctx = (psCipherContext_t*)&lssl->sec.encryptCtx;
	return psDes3Encrypt(ctx, pt, ct, len);	
}

int32 csDes3Decrypt(void *ssl, unsigned char *ct,
					 unsigned char *pt, uint32 len)
{
	ssl_t	*lssl = ssl;
	psCipherContext_t *ctx = (psCipherContext_t*)&lssl->sec.decryptCtx;
	return psDes3Decrypt(ctx, ct, pt, len);	
}

#endif /* USE_3DES_CIPHER_SUITE */
/******************************************************************************/


#ifdef USE_AES_CIPHER_SUITE


/******************************************************************************/
int32 csAesInit(sslSec_t *sec, int32 type, uint32 keysize)
{
	int32	err;
	
	if (type == INIT_ENCRYPT_CIPHER) {	
		memset(&(sec->encryptCtx), 0, sizeof(aes_CBC));	
		if ((err = psAesInit(&(sec->encryptCtx), sec->writeIV, sec->writeKey,
							 keysize)) < 0) {
			return err;
		}
	} else { /* Init for decrypt */	
		memset(&(sec->decryptCtx), 0, sizeof(aes_CBC));	
		if ((err = psAesInit(&(sec->decryptCtx), sec->readIV, sec->readKey,
							 keysize)) < 0) {
			return err;
		}
	}
	return PS_SUCCESS;
}

int32 csAesEncrypt(void *ssl, unsigned char *pt,
					 unsigned char *ct, uint32 len)
{
	ssl_t	*lssl = ssl;
	psCipherContext_t *ctx = (psCipherContext_t*)&lssl->sec.encryptCtx;
	return psAesEncrypt(ctx, pt, ct, len);	
}

int32 csAesDecrypt(void *ssl, unsigned char *ct,
					 unsigned char *pt, uint32 len)
{
	ssl_t	*lssl = ssl;
	psCipherContext_t *ctx = (psCipherContext_t*)&lssl->sec.decryptCtx;
	return psAesDecrypt(ctx, ct, pt, len);	
}
#endif /* USE_AES_CIPHER_SUITE */

/******************************************************************************/



/******************************************************************************/
/*	Null cipher crypto */
/******************************************************************************/
static int32 csNullEncrypt(void *ctx, unsigned char *in,
						 unsigned char *out, uint32 len)
{
	if (out != in) {
		memcpy(out, in, len);
	}
	return len;
}

static int32 csNullDecrypt(void *ctx, unsigned char *in,
						 unsigned char *out, uint32 len)
{
	if (out != in) {
		memmove(out, in, len);
	}
	return len;
}

/******************************************************************************/
/*	HMAC wrappers for cipher suites */
/******************************************************************************/
static int32 csNullGenerateMac(void *ssl, unsigned char type,
						unsigned char *data, uint32 len, unsigned char *mac)
{
	return 0;
}

static int32 csNullVerifyMac(void *ssl, unsigned char type,
						unsigned char *data, uint32 len, unsigned char *mac)
{
	return 0;
}

#ifdef USE_SHA_MAC
/******************************************************************************/
static int32 csShaGenerateMac(void *sslv, unsigned char type,
					unsigned char *data, uint32 len, unsigned char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;

#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
			return tlsHMACSha1(ssl, HMAC_CREATE, type, data, len, mac);
	} else {
#endif /* USE_TLS */
#ifndef DISABLE_SSLV3
		return ssl3HMACSha1(ssl->sec.writeMAC, ssl->sec.seq, type, data,
				len, mac);
#else
		return PS_ARG_FAIL;
#endif /* DISABLE_SSLV3 */
#ifdef USE_TLS
	}
#endif /* USE_TLS */
}

static int32 csShaVerifyMac(void *sslv, unsigned char type,
					unsigned char *data, uint32 len, unsigned char *mac)
{
	int32			hashSize;
	unsigned char	buf[MAX_HASH_SIZE];
	ssl_t	*ssl = (ssl_t*)sslv;
	
	hashSize = SHA1_HASH_SIZE;
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
			tlsHMACSha1(ssl, HMAC_VERIFY, type, data, len, buf);
	} else {
#endif /* USE_TLS */
#ifndef DISABLE_SSLV3
		ssl3HMACSha1(ssl->sec.readMAC, ssl->sec.remSeq, type, data, len, buf);
#endif /* DISABLE_SSLV3 */		
#ifdef USE_TLS
	}
#endif /* USE_TLS */
	if (memcmp(buf, mac, hashSize) == 0) {
		return PS_SUCCESS;
	}
	return PS_FAILURE;
}
#endif /* USE_SHA_MAC */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_MD5_MAC
/******************************************************************************/
static int32 csMd5GenerateMac(void *sslv, unsigned char type,
					unsigned char *data, uint32 len, unsigned char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		return tlsHMACMd5(ssl, HMAC_CREATE, type, data, len, mac);
	} else {
#endif /* USE_TLS */
#ifndef DISABLE_SSLV3
		return ssl3HMACMd5(ssl->sec.writeMAC, ssl->sec.seq, type, data,
						   len, mac);
#else
		return PS_ARG_FAIL;
#endif /* DISABLE_SSLV3 */						   
#ifdef USE_TLS
	}
#endif /* USE_TLS */
}

static int32 csMd5VerifyMac(void *sslv, unsigned char type, unsigned char *data,
					uint32 len, unsigned char *mac)
{
	unsigned char	buf[MD5_HASH_SIZE];
	ssl_t	*ssl = (ssl_t*)sslv;
	
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		tlsHMACMd5(ssl, HMAC_VERIFY, type, data, len, buf);
	} else {
#endif /* USE_TLS */
#ifndef DISABLE_SSLV3
		ssl3HMACMd5(ssl->sec.readMAC, ssl->sec.remSeq, type, data, len, buf);
#endif /* DISABLE_SSLV3 */		
#ifdef USE_TLS
	}
#endif /* USE_TLS */
	if (memcmp(buf, mac, MD5_HASH_SIZE) == 0) {
		return PS_SUCCESS;
	}
	return PS_FAILURE;
}
#endif /* USE_MD5_MAC */
/******************************************************************************/

/******************************************************************************/
/*	Public Key operations wrappers for cipher suites */
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int32 csRsaEncryptPub(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data)
{
	psAssert(key->type == PS_RSA);
	return psRsaEncryptPub(pool, (psRsaKey_t*)key->key, in, inlen, out, outlen,
			data);
}

int32 csRsaDecryptPub(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data)
{
	psAssert(key->type == PS_RSA);
	return psRsaDecryptPub(pool, (psRsaKey_t*)key->key, in, inlen, out, outlen,
			data);
}

int32 csRsaEncryptPriv(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data)
{
	psAssert(key->type == PS_RSA);
	return psRsaEncryptPriv(pool, (psRsaKey_t*)key->key, in, inlen, out,
		outlen, data);
}

int32 csRsaDecryptPriv(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen,
			void *data)
{
	psAssert(key->type == PS_RSA);
	return psRsaDecryptPriv(pool, (psRsaKey_t*)key->key, in, inlen, out, outlen,
			data);
}



/******************************************************************************/


static sslCipherSpec_t	supportedCiphers[] = {
/*
	New ciphers should be added here, similar to the ones below

	Ciphers are listed in order of greater security at top... this generally
	means the slower ones are on top as well.
*/

#ifdef USE_TLS_RSA_WITH_AES_256_CBC_SHA
	{TLS_RSA_WITH_AES_256_CBC_SHA,
		CS_RSA,
		CRYPTO_FLAGS_AES256 | CRYPTO_FLAGS_SHA1,
		20,			/* macSize */
		32,			/* keySize */
		16,			/* ivSize */
		16,			/* blocksize */
		csAesInit,
		csAesEncrypt, 
		csAesDecrypt, 
		csShaGenerateMac, 
		csShaVerifyMac},
#endif /* USE_TLS_RSA_WITH_AES_256_CBC_SHA */

#ifdef USE_TLS_RSA_WITH_AES_128_CBC_SHA
	{TLS_RSA_WITH_AES_128_CBC_SHA,
		CS_RSA,
		CRYPTO_FLAGS_AES | CRYPTO_FLAGS_SHA1,
		20,			/* macSize */
		16,			/* keySize */
		16,			/* ivSize */
		16,			/* blocksize */
		csAesInit,
		csAesEncrypt, 
		csAesDecrypt,  
		csShaGenerateMac, 
		csShaVerifyMac},
#endif /* USE_TLS_RSA_WITH_AES_128_CBC_SHA */


#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
	{SSL_RSA_WITH_3DES_EDE_CBC_SHA,
		CS_RSA,
		CRYPTO_FLAGS_3DES | CRYPTO_FLAGS_SHA1,
		20,			/* macSize */
		24,			/* keySize */
		8,			/* ivSize */
		8,			/* blocksize */
		csDes3Init,
		csDes3Encrypt, 
		csDes3Decrypt,  
		csShaGenerateMac, 
		csShaVerifyMac},
#endif /* USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */

#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
	{SSL_RSA_WITH_RC4_128_SHA,
		CS_RSA,
		CRYPTO_FLAGS_ARC4 | CRYPTO_FLAGS_SHA1,
		20,			/* macSize */
		16,			/* keySize */
		0,			/* ivSize */
		1,			/* blocksize */
		csArc4Init,
		csArc4Encrypt, 
		csArc4Decrypt, 
		csShaGenerateMac, 
		csShaVerifyMac},
#endif /* USE_SSL_RSA_WITH_RC4_128_SHA */

#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
	{SSL_RSA_WITH_RC4_128_MD5,
		CS_RSA,
		CRYPTO_FLAGS_ARC4 | CRYPTO_FLAGS_MD5,
		16,			/* macSize */
		16,			/* keySize */
		0,			/* ivSize */
		1,			/* blocksize */
		csArc4Init,
		csArc4Encrypt, 
		csArc4Decrypt,  
		csMd5GenerateMac, 
		csMd5VerifyMac},
#endif /* USE_SSL_RSA_WITH_RC4_128_MD5 */


/*
	These two USE_SSL_RSA_WITH_NULL ciphers are not recommended for use
	in production applications.
*/
#ifdef USE_SSL_RSA_WITH_NULL_SHA
	{SSL_RSA_WITH_NULL_SHA,
		CS_RSA,
		CRYPTO_FLAGS_SHA1,
		20,			/* macSize */
		0,			/* keySize */
		0,			/* ivSize */
		0,			/* blocksize */
		csNullInit,
		csNullEncrypt, 
		csNullDecrypt, 
		csShaGenerateMac, 
		csShaVerifyMac},
#endif /* USE_SSL_RSA_WITH_NULL_SHA */

#ifdef USE_SSL_RSA_WITH_NULL_MD5
	{SSL_RSA_WITH_NULL_MD5,
		CS_RSA,
		CRYPTO_FLAGS_MD5,
		16,			/* macSize */
		0,			/* keySize */
		0,			/* ivSize */
		0,			/* blocksize */
		csNullInit,
		csNullEncrypt, 
		csNullDecrypt,  
		csMd5GenerateMac, 
		csMd5VerifyMac},
#endif /* USE_SSL_RSA_WITH_NULL_MD5 */

/*
	The NULL Cipher suite must exist and be the last in this list
*/
	{SSL_NULL_WITH_NULL_NULL,
		CS_NULL,
		0,
		0,
		0,
		0,
		0,
		csNullInit,
		csNullEncrypt, 
		csNullDecrypt, 
		csNullGenerateMac, 
		csNullVerifyMac}
};

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Disable and re-enable ciphers suites on a global or per-session level.
	This is only a server-side feature because the client is always able to
	nominate the specific cipher it wishes to use.  Servers may want to disable
	specific ciphers for a given session (or globally without having to
	rebuild the library).
	
	This function must be called immediately after matrixSslNewServerSession
	
	If ssl is NULL, the setting will be global.  If a cipher is globally
	disabled, the per-session setting will be ignored.
	
	flags:
		PS_TRUE to reenable (always enabled by default if compiled in)
		PS_FALSE to disable cipher suite
*/
int32 matrixSslSetCipherSuiteEnabledStatus(ssl_t *ssl, uint16 cipherId,
			uint32 flags)
{
	uint16	i, j;
	
	if (ssl && !(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_UNSUPPORTED_FAIL;
	}
	if (flags != PS_TRUE && flags != PS_FALSE) {
		return PS_ARG_FAIL;
	}
	for (i = 0; supportedCiphers[i].ident != SSL_NULL_WITH_NULL_NULL; i++) {
		if (supportedCiphers[i].ident == cipherId) {
			if (ssl == NULL) {
/*
				Global status of cipher suite.  Disabled status takes
				precident over session setting
*/
				if (flags == PS_TRUE) {
					supportedCiphers[i].flags &= ~CRYPTO_FLAGS_DISABLED;
				} else {
					supportedCiphers[i].flags |= CRYPTO_FLAGS_DISABLED;
				}
				return PS_SUCCESS;
			} else {
/*
				Status of this suite for a specific session
*/
				for (j = 0; j < SSL_MAX_DISABLED_CIPHERS; j++) {
					if (flags == PS_FALSE) {
						/* Find first empty spot to add disabled cipher */
						if (ssl->disabledCiphers[j] == 0x0 ||
								ssl->disabledCiphers[j] == cipherId) {
							ssl->disabledCiphers[j] = cipherId;
							return PS_SUCCESS;
						}
					} else {
						if (ssl->disabledCiphers[j] == cipherId) {
							ssl->disabledCiphers[j] = 0x0;
							return PS_SUCCESS;
						}
					}
				}
				if (flags == PS_FALSE) {
					return PS_LIMIT_FAIL; /* No empty spot in disabledCiphers */
				} else {
					/* Tried to re-enabled a cipher that wasn't disabled */
					return PS_SUCCESS; 
				}
			}
		}
	}
	return PS_FAILURE; /* Cipher not found */
}
#endif /* USE_SERVER_SIDE_SSL */

#ifdef VALIDATE_KEY_MATERIAL
#define KEY_ALG_ANY		1
#define KEY_ALG_FIRST	2
/*
	anyOrFirst is basically a determination of whether we are looking through
	a collection of CA files for an algorithm (ANY) or a cert chain where
	we really only care about the child most cert because that is the one
	that ultimately determines the authentication algorithm (FIRST)
*/
static int32 haveCorrectKeyAlg(psX509Cert_t *cert, int32 keyAlg, int anyOrFirst)
{		
	while (cert) {
		if (cert->pubKeyAlgorithm == keyAlg) {
			return PS_SUCCESS;
		}
		if (anyOrFirst == KEY_ALG_FIRST) {
			return PS_FAILURE;
		}
		cert = cert->next;
	}
	return PS_FAILURE;
}

/*
	This is the signature algorithm that the client will be using to encrypt
	the key material based on what the cipher suite says it should be.
	Only looking at child most cert
*/
static int32 haveCorrectSigAlg(psX509Cert_t *cert, int32 sigType)
{		
	if (sigType == RSA_TYPE_SIG) {
		if (cert->sigAlgorithm == OID_SHA1_RSA_SIG ||
				cert->sigAlgorithm == OID_SHA256_RSA_SIG ||
				cert->sigAlgorithm == OID_SHA384_RSA_SIG ||
				cert->sigAlgorithm == OID_SHA512_RSA_SIG ||
				cert->sigAlgorithm == OID_MD5_RSA_SIG ||
				cert->sigAlgorithm == OID_MD2_RSA_SIG) {
			return PS_SUCCESS;
		}
	} else if (sigType == DSA_TYPE_SIG) {
		if (cert->sigAlgorithm == OID_SHA1_ECDSA_SIG ||
				cert->sigAlgorithm == OID_SHA224_ECDSA_SIG ||
				cert->sigAlgorithm == OID_SHA256_ECDSA_SIG ||
				cert->sigAlgorithm == OID_SHA384_ECDSA_SIG ||
				cert->sigAlgorithm == OID_SHA512_ECDSA_SIG) {
			return PS_SUCCESS;
		}
	}

	return PS_FAILURE;
}


#ifdef USE_CLIENT_SIDE_SSL
/* Test if agreed upon cipher suite authentication is being adhered to */
int32 csCheckCertAgainstCipherSuite(int32 sigAlg, int32 cipherType)
{
	if (sigAlg == OID_MD5_RSA_SIG || sigAlg == OID_SHA1_RSA_SIG ||
			sigAlg == OID_SHA256_RSA_SIG || sigAlg == OID_SHA384_RSA_SIG ||
			sigAlg == OID_SHA512_RSA_SIG) {
		if (cipherType == CS_DHE_RSA || cipherType == CS_RSA ||
				cipherType == CS_ECDHE_RSA || cipherType == CS_ECDH_RSA) {
			return 1;
		}
	}
	if (sigAlg == OID_SHA1_ECDSA_SIG || sigAlg == OID_SHA224_ECDSA_SIG ||
			sigAlg == OID_SHA256_ECDSA_SIG || sigAlg == OID_SHA384_ECDSA_SIG ||
			sigAlg == OID_SHA512_ECDSA_SIG) {
		if (cipherType == CS_ECDHE_ECDSA || cipherType == CS_ECDH_ECDSA) {
			return 1;
		}
	
	}
	return 0; /* no match */
}					
#endif /* USE_CLIENT_SIDE_SSL */
					
/******************************************************************************/
/*
	Don't report a matching cipher suite if the user hasn't loaded the 
	proper public key material to support it.  We do not check the client
	auth side of the algorithms because that authentication mechanism is
	negotiated within the handshake itself
	
	The annoying #ifdef USE_SERVER_SIDE and CLIENT_SIDE are because the
	structure members only exist one one side or the other and so are used
	for compiling.  You can't actually get into the wrong area of the
	SSL_FLAGS_SERVER test so no #else cases should be needed
 */
static int32 haveKeyMaterial(ssl_t *ssl, int32 cipherType)
{

	/*	To start, capture all the cipherTypes where servers must have an
		identity and clients have a CA so we don't repeat them everywhere */
	if (cipherType == CS_RSA || cipherType == CS_DHE_RSA ||
			cipherType == CS_ECDHE_RSA || cipherType == CS_ECDH_RSA ||
			cipherType == CS_ECDHE_ECDSA || cipherType == CS_ECDH_ECDSA) {
		if (ssl->flags & SSL_FLAGS_SERVER) {
#ifdef USE_SERVER_SIDE_SSL	
			if (ssl->keys == NULL || ssl->keys->cert == NULL ||
					ssl->keys->privKey == NULL) {
				return PS_FAILURE;
			}
#endif
#ifdef USE_CLIENT_SIDE_SSL
		} else {	
			if (ssl->keys == NULL || ssl->keys->CAcerts == NULL) {
				return PS_FAILURE;
			}
#endif				
		}
	}
	
	/*	Standard RSA ciphers types - auth and exchange */
	if (cipherType == CS_RSA) {
		if (ssl->flags & SSL_FLAGS_SERVER) {
#ifdef USE_SERVER_SIDE_SSL			
			if (haveCorrectKeyAlg(ssl->keys->cert, OID_RSA_KEY_ALG,
					KEY_ALG_FIRST) < 0) {
				return PS_FAILURE;
			}
			if (haveCorrectSigAlg(ssl->keys->cert, RSA_TYPE_SIG) < 0) {
				return PS_FAILURE;
			}
#endif
#ifdef USE_CLIENT_SIDE_SSL			
		} else { /* Client */
		
			if (haveCorrectKeyAlg(ssl->keys->CAcerts, OID_RSA_KEY_ALG,
					KEY_ALG_ANY) < 0) {
				return PS_FAILURE;
			}
#endif			
		}
	}
	


	 
	return PS_SUCCESS;
}
#endif /* VALIDATE_KEY_MATERIAL */

/******************************************************************************/
/*
	Lookup the given cipher spec ID and return a pointer to the structure
	if found.  This is used when negotiating security, to find out what suites
	we support.
*/
sslCipherSpec_t *sslGetCipherSpec(ssl_t *ssl, uint32 id)
{
	int16	i;
#ifdef USE_SERVER_SIDE_SSL
	int16	j;
#endif /* USE_SERVER_SIDE_SSL */

	i = 0;
	do {
		if (supportedCiphers[i].ident == id) {
#ifdef USE_SERVER_SIDE_SSL		
/*
			Globally disabled?
*/
			if (supportedCiphers[i].flags & CRYPTO_FLAGS_DISABLED) {
				psTraceIntInfo("Matched cipher suite %d but disabled by user\n",
					id);
				return NULL;
			}
/*
			Disabled for session?
*/
			if (id != 0) { /* Disable NULL_WITH_NULL_NULL not possible */
				for (j = 0; j < SSL_MAX_DISABLED_CIPHERS; j++) {
					if (ssl->disabledCiphers[j] == id) {
						psTraceIntInfo("Matched cipher suite %d but disabled by user\n",
							id);
						return NULL;
					}
				}
			}
#endif /* USE_SERVER_SIDE_SSL */			
/*
			Unusable due to key material not available?
*/
#ifdef VALIDATE_KEY_MATERIAL
			if (haveKeyMaterial(ssl, supportedCiphers[i].type) == PS_SUCCESS) {
				return &supportedCiphers[i];
			}
			psTraceIntInfo("Matched cipher suite %d but no supporting keys\n",
				id);
#else
			return &supportedCiphers[i];
#endif /* VALIDATE_KEY_MATERIAL */
		}
	} while (supportedCiphers[i++].ident != SSL_NULL_WITH_NULL_NULL) ;

	return NULL;
}


/******************************************************************************/
/*
	Write out a list of the supported cipher suites to the caller's buffer
	First 2 bytes are the number of cipher suite bytes, the remaining bytes are
	the cipher suites, as two byte, network byte order values.
*/
int32 sslGetCipherSpecList(ssl_t *ssl, unsigned char *c, int32 len,
		int32 addScsv)
{
	unsigned char	*end, *p;
	unsigned short	i;
	int32			ignored;

	if (len < 4) {
		return -1;
	}
	end = c + len;
	p = c; c += 2;
	
	ignored = 0;
	for (i = 0; supportedCiphers[i].ident != SSL_NULL_WITH_NULL_NULL; i++) {
		if (end - c < 2) {
			return -1;
		}
#ifdef VALIDATE_KEY_MATERIAL
		if (haveKeyMaterial(ssl, supportedCiphers[i].type) != PS_SUCCESS) {
			ignored += 2;
			continue;
		}
#endif 		
		*c = (unsigned char)((supportedCiphers[i].ident & 0xFF00) >> 8); c++;
		*c = (unsigned char)(supportedCiphers[i].ident & 0xFF); c++;
	}
	i *= 2;
	i -= (unsigned short)ignored;
#ifdef ENABLE_SECURE_REHANDSHAKES
	if (addScsv == 1) {
		*c = ((TLS_EMPTY_RENEGOTIATION_INFO_SCSV & 0xFF00) >> 8); c++;
		*c = TLS_EMPTY_RENEGOTIATION_INFO_SCSV  & 0xFF; c++;
		i += 2;
	}
#endif	
	*p = (unsigned char)(i >> 8); p++;
	*p = (unsigned char)(i & 0xFF);
	return i + 2;
}

/******************************************************************************/
/*
	Return the length of the cipher spec list, including initial length bytes,
	(minus any suites that we don't have the key material to support)
*/
int32 sslGetCipherSpecListLen(ssl_t *ssl)
{
	int32	i, ignored;

	ignored = 0;
	for (i = 0; supportedCiphers[i].ident != SSL_NULL_WITH_NULL_NULL; i++) {
#ifdef VALIDATE_KEY_MATERIAL
		if (haveKeyMaterial(ssl, supportedCiphers[i].type) != PS_SUCCESS) {
			ignored += 2;
		}
#endif 	
	}
	return (i * 2) + 2 - ignored;
}

/******************************************************************************/

