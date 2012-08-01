/*
 *	cryptoApi.h
 *	Release $Name: MATRIXSSL-3-3-1-OPEN $
 *
 *	Prototypes for the Matrix crypto public APIs
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

#ifndef _h_PS_CRYPTOAPI
#define _h_PS_CRYPTOAPI

#include "../core/coreApi.h" /* Must be first included */
#include "cryptoConfig.h" /* Must be second included */

#include "cryptolib.h"

/******************************************************************************/
/*	Public return codes */
/******************************************************************************/
/*	Failure codses MUST be < 0  */
/*	NOTE: The range for crypto error codes must be between -30 and -49  */
#define	PS_PARSE_FAIL			-31

/*
	PS NOTE:  Any future additions to certificate authentication failures
	must be carried through to MatrixSSL code
*/	
#define PS_CERT_AUTH_PASS	PS_TRUE
#define	PS_CERT_AUTH_FAIL_BC	-32 /* BasicConstraint failure */
#define	PS_CERT_AUTH_FAIL_DN	-33 /* DistinguishedName failure */
#define	PS_CERT_AUTH_FAIL_SIG	-34 /* Signature validation failure */
#define	PS_CERT_AUTH_FAIL		-35 /* Generic cert auth fail */

#define PS_SIGNATURE_MISMATCH	-36 /* Alorithms all work but sig not a match */

/******************************************************************************/
/* Public APIs */
/******************************************************************************/
#ifdef USE_AES
/******************************************************************************/
/*
	CBC Mode AES
*/
PSPUBLIC int32 psAesInit(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psAesDecrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psAesEncrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
/*
	Block Mode AES
*/
PSPUBLIC int32 psAesInitKey(const unsigned char *key, uint32 keylen,
						psAesKey_t *skey);
PSPUBLIC void psAesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psAesKey_t *skey);
PSPUBLIC void psAesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psAesKey_t *skey);


#endif /* USE_AES */
/******************************************************************************/

/******************************************************************************/

/******************************************************************************/
#ifdef USE_3DES
/******************************************************************************/
/*
	CBC Mode DES3 
*/
PSPUBLIC int32 psDes3Init(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psDes3Decrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psDes3Encrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
/*
	Block Mode DES3 
*/
PSPUBLIC int32 psDes3InitKey(const unsigned char *key, uint32 keylen,
						psDes3Key_t *skey);
PSPUBLIC void psDes3EncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *skey);
PSPUBLIC void psDes3DecryptBlock(const unsigned char *ct, unsigned char *pt,
						psDes3Key_t *skey);
#endif /* USE_3DES */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_DES
PSPUBLIC int32 psDesInitKey(const unsigned char *key, int32 keylen,
						psDes3Key_t *skey);
PSPUBLIC void psDesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *skey);
PSPUBLIC void psDesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psDes3Key_t *skey);
#endif /* USE_DES */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_ARC4
PSPUBLIC void psArc4Init(psCipherContext_t *ctx, unsigned char *key,
						uint32 keylen);
PSPUBLIC int32 psArc4(psCipherContext_t *ctx, unsigned char *in,
						unsigned char *out, uint32 len);
#endif /* USE_ARC4 */
/******************************************************************************/

#ifdef USE_RC2
/******************************************************************************/
PSPUBLIC int32 psRc2Init(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psRc2Decrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psRc2Encrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
PSPUBLIC int32 psRc2InitKey(unsigned char *key, uint32 keylen, uint32 rds,
						psRc2Key_t *skey);
PSPUBLIC int32 psRc2EncryptBlock(unsigned char *pt, unsigned char *ct,
						psRc2Key_t *skey);
PSPUBLIC int32 psRc2DecryptBlock(unsigned char *ct, unsigned char *pt,
						psRc2Key_t *skey);					
#endif /* USE_RC2 */
/******************************************************************************/
/******************************************************************************/
#ifdef USE_SHA1
/******************************************************************************/
PSPUBLIC void psSha1Init(psDigestContext_t * md);
PSPUBLIC void psSha1Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psSha1Final(psDigestContext_t * md, unsigned char *hash);

#ifdef USE_HMAC
PSPUBLIC int32 psHmacSha1(unsigned char *key, uint32 keyLen,
				const unsigned char *buf, uint32 len,
				unsigned char *hash, unsigned char *hmacKey,
				uint32 *hmacKeyLen);
PSPUBLIC void psHmacSha1Init(psHmacContext_t *ctx, unsigned char *key,
				uint32 keyLen);
PSPUBLIC void psHmacSha1Update(psHmacContext_t *ctx, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psHmacSha1Final(psHmacContext_t *ctx, unsigned char *hash); 
#endif /* USE_HMAC */
#endif /* USE_SHA1 */
/******************************************************************************/


/******************************************************************************/
#ifdef USE_MD5
/******************************************************************************/
PSPUBLIC void psMd5Init(psDigestContext_t * md);
PSPUBLIC void psMd5Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psMd5Final(psDigestContext_t * md, unsigned char *hash);

#ifdef USE_HMAC
PSPUBLIC int32 psHmacMd5(unsigned char *key, uint32 keyLen,
				const unsigned char *buf, uint32 len,
				unsigned char *hash, unsigned char *hmacKey,
				uint32 *hmacKeyLen);
PSPUBLIC void psHmacMd5Init(psHmacContext_t *ctx, unsigned char *key,
				uint32 keyLen);
PSPUBLIC void psHmacMd5Update(psHmacContext_t *ctx, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psHmacMd5Final(psHmacContext_t *ctx, unsigned char *hash); 
#endif /* USE_HMAC */
#endif /* USE_MD5 */
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
#ifdef USE_RSA
/******************************************************************************/
/*
	Private Key Parsing
	PKCS#1 - RSA specific
	PKCS#8 - General private key storage format
*/
#ifdef USE_PRIVATE_KEY_PARSING
PSPUBLIC int32 pkcs1ParsePrivBin(psPool_t *pool, unsigned char *p,
				uint32 size, psPubKey_t **key);
#ifdef USE_PKCS8
PSPUBLIC int32 pkcs8ParsePrivBin(psPool_t *pool, unsigned char *p,
				int32 size, char *pass, psPubKey_t **key);
#ifdef MATRIX_USE_FILE_SYSTEM
#ifdef USE_PKCS12				
PSPUBLIC int32 psPkcs12Parse(psPool_t *pool, psX509Cert_t **cert,
				psPubKey_t **privKey, const unsigned char *file, int32 flags,
				unsigned char *importPass, int32 ipasslen,
				unsigned char *privkeyPass, int32 kpasslen);
#endif /* USE_PKCS12 */
#endif /* MATRIX_USE_FILE_SYSTEM */				
#endif /* USE_PKCS8 */

#ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32 pkcs1ParsePrivFile(psPool_t *pool, char *fileName,
				char *password, psPubKey_t **outkey);
PSPUBLIC int32 pkcs1DecodePrivFile(psPool_t *pool, char *fileName,
				char *password,	unsigned char **DERout, uint32 *DERlen);
#endif /* MATRIX_USE_FILE_SYSTEM */
#endif /* USE_PRIVATE_KEY_PARSING */

#endif /* USE_RSA */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_PKCS5
/******************************************************************************/
/*
	PKCS#5 PBKDF v1 and v2 key generation
*/
PSPUBLIC void pkcs5pbkdf1(unsigned char *pass, uint32 passlen,
				unsigned char *salt, int32 iter, unsigned char *key);
PSPUBLIC void pkcs5pbkdf2(unsigned char *password, uint32 pLen, 
				 unsigned char *salt, uint32 sLen, int32 rounds,
				 unsigned char *key, uint32 kLen);
#endif /* USE_PKCS5 */

/******************************************************************************/
/*
	Public Key Cryptography
*/
PSPUBLIC psPubKey_t *psNewPubKey(psPool_t *pool);
PSPUBLIC void psFreePubKey(psPubKey_t *key);

/******************************************************************************/
#ifdef USE_RSA
/******************************************************************************/
/*
	RSA crypto
*/

PSPUBLIC int32 psRsaDecryptPriv(psPool_t *pool, psRsaKey_t *key,
					unsigned char *in, uint32 inlen,
					unsigned char *out, uint32 outlen, void *data);
PSPUBLIC int32 psRsaDecryptPub(psPool_t *pool, psRsaKey_t *key,
					unsigned char *in, uint32 inlen,
					unsigned char *out, uint32 outlen, void *data);
PSPUBLIC int32 psRsaEncryptPub(psPool_t *pool, psRsaKey_t *key,
				unsigned char *in, uint32 inlen,
				unsigned char *out, uint32 outlen, void *data);
PSPUBLIC int32 psRsaCrypt(psPool_t *pool, const unsigned char *in, uint32 inlen,
				unsigned char *out, uint32 *outlen,	psRsaKey_t *key,
				int32 type, void *data);
#endif /* USE_RSA */
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
#ifdef USE_X509
/******************************************************************************/
/*
	X.509 Certificate support	
*/
PSPUBLIC int32 psX509ParseCertFile(psPool_t *pool, char *fileName,
					psX509Cert_t **outcert, int32 flags);
PSPUBLIC int32 psX509ParseCert(psPool_t *pool, unsigned char *pp, uint32 size,
					psX509Cert_t **outcert, int32 flags);
PSPUBLIC void psX509FreeCert(psX509Cert_t *cert);
#ifdef USE_CERT_PARSE
PSPUBLIC int32 psX509AuthenticateCert(psPool_t *pool, psX509Cert_t *subjectCert,
					psX509Cert_t *issuerCert);
#endif
#endif /* USE_X509 */
/******************************************************************************/

/******************************************************************************/
PSPUBLIC int32 psInitPrng(psRandom_t *ctx);
PSPUBLIC int32 psGetPrng(psRandom_t *ctx, unsigned char *bytes, uint32 size);
				

#ifdef USE_YARROW
PSPUBLIC int32 psYarrowStart(psYarrow_t *ctx);
PSPUBLIC int32 psYarrowAddEntropy(unsigned char *in, uint32 inlen,
			psYarrow_t *prng);
PSPUBLIC int32 psYarrowReseed(psYarrow_t *ctx);
PSPUBLIC uint32 psYarrowRead(unsigned char *out, uint32 outlen, psYarrow_t *cx);
PSPUBLIC int32 psYarrowDone(psYarrow_t *ctx);
PSPUBLIC int32 psYarrowExport(unsigned char *out, uint32 *outlen,
			psYarrow_t *ctx);
PSPUBLIC int32 psYarrowImport(unsigned char *in, uint32 inlen, psYarrow_t *ctx);
#endif /* USE_YARROW */
/******************************************************************************/

#endif /* _h_PS_CRYPTOAPI */
/******************************************************************************/

